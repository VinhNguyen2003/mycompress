# mycompress
## Overview
I got tired of remembering all the commands for compressing files in UNIX so I created a program that does all of it.

This project is built upon a project in CSCI 4061. I intially started on this project to get a better understanding of file streams and IO. Also, yes, there are programs that exist to handle multiple compression formats, but they are mostly interacted with via a GUI. Nowadays, I find myself working from the terminal more and more, and remembering all the different commands to extract different formats is a pain. Anyways, I hope that you enjoy this little project of mine.

## Usage
Once installed, you can run `mycompress` from the terminal.

Running `mycompress` without any arguments will display:
```
Usage: ./mycompress <format> <command> <archive_name> [-o output_dir] [file...]
Formats: tar, targz, zip, 7z
Commands: -c (create), -x (extract)
```

## Installation

To install `mycompress` on your system, follow these steps:

1. Clone the repository
2. Navigate to the cloned directory
3. Run the installation script
   ```
   git clone https://github.com/VinhNguyen2003/mycompress.git
   cd mycompress
   ./install.sh
   ```
## Change Log
- 3/1/2024: tar and tar.gz fully supported, user can set output path.
- 4/1/2024: zip and 7zip fully supported, bug fixes.
