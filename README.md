# FilePatcher
Bytepatch Files

## How to use

 1. Normal
 2. Using Arguments

### 1. Normal Usage
Just start the .exe and it will guide you through the patch process.

### 2. Using Arguments
FilePatcher.exe fileName Type Offset/Signature Offoffset (only Signature) patchSize readOnly patchBytes
#### Explanation:

 - **File Name:**
 The path to the file to patch (relative to the FilePatcher.exe)
 - **Type:**
The Patch Type (1 = Offset, 2 = Signature)
 - **Offset/Signature:**
 The Address/Signature Pattern you want to patch (address in hex)
  - **Offoffset:**
 How much you want to offset the found address (only used with Signature)
 - **Patch Size:**
The amount of bytes you want to patch/read
 - **Read only:**
If active prints the amount of bytes at the address
 - **Patch Bytes:**
The Bytes that are written (in hex)
#### Examples:
 - **Offset:**
FilePatcher.exe patch.dll 400 2 0 90 90
- **Signature:**
FilePatcher.exe patch.dll "0F 84 ? ? ? ?" 2 4 0 90 90 90 90
