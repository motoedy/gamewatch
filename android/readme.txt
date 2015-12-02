Download the appropriate SDL module sources (SDL, SDL mixer, SDL ttf)

* Create the required links in the 'jni' folder:
	ln -s ..../SDL2-x.y.z ./jni/SDL2
	ln -s ..../SDL2_mixer-x.y.z ./jni/SDL2_mixer
	ln -s ..../SDL2_ttf-x.y.z ./jni/SDL2_ttf
* Prepere all resource files under the 'assets/data' folder:
	extract all zip files from 'trunk/data' into assets/data
* run 'ndk-build.cmd'
