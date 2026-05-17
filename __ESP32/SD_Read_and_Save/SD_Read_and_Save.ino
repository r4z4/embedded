#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CS_PIN 3
#define MAX_PATH_LEN 1024
char latest_dir_num[MAX_PATH_LEN] = ""; // "04" e.g.

int num;
char new_dirname[20]; // "/04" e.g.

File new_dir;
char latest_filename[20]; // "04" e.g.
char new_filename[20]; // "/04" e.g.
char latest_reading[40]; //

int TIME_TO_SLEEP = 300;
unsigned long long uS_TO_S_FACTOR = 1000000;
RTC_DATA_ATTR int bootCount = 0; // Will equal dirname

RTC_DATA_ATTR int latestFilename = 0;

// Read a sensor and save the datally locally to micro sd card

// void generate_latest_dirname() {
//     const char *parent_path = "."; // Current directory for demonstration
//     DIR *dir;
//     struct dirent *entry;
//     struct stat file_stat;
//     time_t latest_mtime = 0;
//     char full_path[MAX_PATH_LEN];

//     dir = opendir(parent_path);
//     if (dir == NULL) {
//         perror("Error opening directory");
//     }

//     while ((entry = readdir(dir)) != NULL) {
//         // Skip "." and ".." entries
//         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
//             continue;
//         }

//         // Construct full path
//         snprintf(full_path, MAX_PATH_LEN, "%s/%s", parent_path, entry->d_name);

//         if (stat(full_path, &file_stat) == -1) {
//             perror("Error getting file status");
//             continue;
//         }

//         // Check if it's a directory
//         if (S_ISDIR(file_stat.st_mode)) {
//             if (file_stat.st_mtime > latest_mtime) {
//                 latest_mtime = file_stat.st_mtime;
//                 strncpy(latest_dir_name, entry->d_name, MAX_PATH_LEN - 1);
//                 latest_dir_name[MAX_PATH_LEN - 1] = '\0'; // Ensure null-termination
//             }
//         }
//     }

//     closedir(dir);

//     if (strlen(latest_dir_name) > 0) {
//         printf("Most recently modified directory: %s\n", latest_dir_name);
//     } else {
//         printf("No subdirectories found or an error occurred.\n");
//     }
// }

void getLatestReading() {
  sprintf(latest_reading, "%s", "44; 77; 88");
}

void scanDirCreateLatest(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  if (!file) {
    sprintf(new_dirname, "/%d", bootCount);
    createDir(SD, new_dirname);
  }
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      num = atoi(file.name());
      if (num != bootCount) {
        break;
      } else {
        num++;
        sprintf(new_dirname, "/%d", num);
        createDir(SD, new_dirname);
      }
      // if(levels){
      //   listDir(fs, file.name(), levels -1);
      // }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void scanDirGetLatestFile(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File local_root = fs.open(dirname);
  if(!local_root){
    Serial.println("Failed to open file");
    return;
  }
  if(!local_root.isDirectory()){
    Serial.println("Not a file");
    return;
  }

  File file = local_root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    } else {
      char originalString[20];
      strcpy(originalString, file.name());
      char delimiter = '.';
      char *substring;
      char *pointerToDelimiter = strrchr(originalString, delimiter);
      int latestFileNum;
      if (pointerToDelimiter != NULL) {
        size_t length = pointerToDelimiter - originalString;
        substring = (char *)malloc(length + 1); 

        if (substring == NULL) {
          perror("Memory allocation failed");
        }

        // Copy the characters before the delimiter
        strncpy(substring, originalString, length);
        // substring[length] = '\0'; // Null-terminate the new string
        
        num = atoi(substring);
        latestFileNum = atoi(latest_filename);
        if (num != latestFileNum) {
          break;
        } else {
          num++;
          sprintf(new_filename, "/%d.txt", num);
        }
        free(substring); // Free allocated memory
      }
      // if(levels){
      //   listDir(fs, file.name(), levels -1);
      // }
    }
    file = local_root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void setup(){
  Serial.begin(115200);
  if(!SD.begin(CS_PIN)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  scanDirCreateLatest(SD, "/", 0);
  // Change to new_dir
  if (chdir(new_dirname) == 0) {
      printf("Directory changed successfully.\n");
  } else {
      perror("chdir");
  }
  // For each reading, write a file. One for now
  // First find latest file in dir
  scanDirGetLatestFile(SD, new_dirname, 0);
  writeFile(SD, new_filename, latest_reading);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  ++bootCount;
  Serial.println("Boot Number: " + String(bootCount));
  Serial.println("I'm tired. Off to sleep ...");
  // Wake up source
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Prepare for sleep
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
  Serial.flush();
  // Enable sleep
  esp_deep_sleep_start();
}

void loop(){

}