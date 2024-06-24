

void getPathStruct(FSSpec *fsspecImage, const char *path) {
	const char*buffer = path;
	if (NativePathNameToFSSpec(buffer, fsspecImage, NULL) != 0) {
		printf("%s not found", buffer);
		exit(4);
	}
}
