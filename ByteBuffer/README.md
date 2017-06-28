例:
```
#include "Things/ByteBuffer/ByteBuffer.h"
void test24() {
	ByteWriter bw;
	bw.writeBool(true);
	bw.writeChar(12);
	bw.writeShort(12345);
	bw.writeInt(1234567890);
	bw.writeInt64(1234567890123);
	bw.writeFloat(1.2345);
	bw.writeString("hello world");
	
	util::File::saveToFile("./test.txt", bw.buffer(), bw.size());
	
	ByteReader br(bw.buffer(), bw.size());
	printf("%d\n", br.readBool());
	printf("%d\n", br.readChar());
	printf("%d\n", br.readShort());
	printf("%d\n", br.readInt());
	printf("%lld\n", br.readInt64());
	printf("%g\n", br.readFloat());
	printf("%s\n", br.readString().c_str());
	
}
```
