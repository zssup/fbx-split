fbx-split:fbx_split.o
	# dynamic
	# g++ build/fbx_split.o -o ./bin/fbx-split -I /usr/local/third_party/fbxsdk/include -L /usr/local/third_party/fbxsdk/lib/gcc/x64/debug -L /usr/lib/x86_64-linux-gnu -l fbxsdk -l xml2
	# static 
	g++ build/fbx_split.o -o ./bin/fbx-split -I /usr/local/third_party/fbxsdk/include -L /usr/local/third_party/fbxsdk/lib/gcc/x64/debug -L /usr/lib/x86_64-linux-gnu -static  -l fbxsdk -l xml2 -l stdc++ -l m -l gcc -l c -l pthread -l icuuc -l z -l lzma -l icudata

fbx_split.o:src/fbx_split.cpp
	g++ -c src/fbx_split.cpp -I /usr/local/third_party/fbxsdk/include -o build/fbx_split.o

.PHONY:clean
clean:
	-rm build/*.o bin/*
