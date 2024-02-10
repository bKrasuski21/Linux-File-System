/*#include <iostream>
#include "Wad.h"
#include <cstdint>

int main() {
    string path = "P3_files/sample1.wad";
    Wad* wad = Wad::loadWad(path);

    return 0;
}*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <string>
#include <libgen.h>
#include "../libWad/Wad.h"

static int do_getattr( const char *path, struct stat *stbuf ){
    memset(stbuf, 0, sizeof(struct stat));
    Wad* wad = ((Wad*)fuse_get_context()->private_data);

    if (wad->isDirectory(path)){
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (wad->isContent(path)){
    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;

    stbuf->st_size = wad->getSize(path);
    return 0;
    }

    return -ENOENT;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    Wad* wad = ((Wad*)fuse_get_context()->private_data);

    std::vector<std::string> wadContents;
    wad->getDirectory(path, &wadContents);

    for (std::vector<std::string>::iterator it = wadContents.begin(); it != wadContents.end(); ++it){
        filler(buffer, it->c_str(), NULL, 0);
    }

    return 0;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi ){
    Wad* wad = ((Wad*)fuse_get_context()->private_data);
    if (wad->isContent(path) == false){
        return -ENOENT;
    }

    int ret = wad->getContents(path, buffer, size, offset);
    return ret;
}

static int do_mkdir( const char *path, mode_t mode )
{
    Wad* wad = ((Wad*)fuse_get_context()->private_data);
    wad->createDirectory(path);
	return 0;
}

static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{
    Wad* wad = ((Wad*)fuse_get_context()->private_data);
    wad->createFile(path);
	return 0;
}

static int do_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info )
{
    Wad* wad = ((Wad*)fuse_get_context()->private_data);
    wad->writeToFile(path, buffer, size, offset);
	return size;
}

static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .mknod		= do_mknod,
    .mkdir		= do_mkdir,
    .read		= do_read,
    .write		= do_write,
    .readdir	= do_readdir,
};

int main(int argc, char* argv[]){
    if (argc < 3) {
        std::cout << "Not enough arguments" << std::endl;
        exit(EXIT_SUCCESS);
    }
    std::string wadPath = argv[argc-2];

    if (wadPath.at(0) != '/'){
        wadPath = std::string(get_current_dir_name()) + "/" + wadPath;
    }

    Wad* myWad = Wad::loadWad(wadPath);

    argv[argc-2] = argv[argc-1];

    argc--;

    return fuse_main(argc, argv, &operations, myWad);

}

