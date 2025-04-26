# RawCopy

Copy locked or read-only files from a disk volume by using raw cluster reading.

## Functionality

- Copies locked or inaccessible files directly from the volume.
- Reads the file's clusters at the file system level to perform the copy.

## Usage

### Running in CMD

1. Open a **Command Prompt** (CMD) window with **administrator privileges**.

2. Run the program with the following command:

   ```bash
   RawCopy.exe <SourcePath> <DestinationPath>
   RawCopy.exe "C:\Windows\System32\config\System.LOG1" "C:\Users\diff\Desktop\SystemLog.txt"

