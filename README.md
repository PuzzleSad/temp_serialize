
idea
save a lot of space with some basic serialization techniques

Each temperature measurement contains
UUID
Time
Temperature

Potentially also a type-id/other identifier

Aims:
        store as binary data instead of plaintext


File contains a header. The header contains some basic information on the details of the stored data.
Data is stored as "sections"

Each section has a header.
The header is simply a struct that contains the base time and the offset to the next header.

Time is then stored as a 16bit integer
65536 states in 2^16
65536 seconds 
1092 minutes
18.2 hours

Thus, each 18 hours we can have a new struct. 
The time stored in the header is the time base
and each entry's time is thus an offset of 
this base.

For this specific file, I will store one file 
per sensor, thus the UUID of the sensor is 
stored in the header of the file.

Each reading is thus like
2 bytes timeOffset
4-8 bytes temp

each header is 16 bytes
address/offset to next
time base















