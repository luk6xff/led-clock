# SCRIPT TO GZIP CRITICAL FILES FOR ACCELERATED WEBSERVING
# see also https://community.platformio.org/t/question-esp32-compress-files-in-data-to-gzip-before-upload-possible-to-spiffs/6274/10
#

Import( 'env', 'projenv' )

import os
import gzip
import shutil
import glob

# HELPER TO GZIP A FILE
def gzip_file(src_path, dst_path):
    with open(src_path, 'rb') as src, gzip.open(dst_path, 'wb') as dst:
        for chunk in iter( lambda: src.read(4096), b"" ):
            dst.write( chunk )

# GZIP DEFINED FILES FROM 'data' DIR to 'data/gzip/' DIR
def gzip_webfiles(source, target, env):

    # FILETYPES / SUFFIXES WHICh NEED TO BE GZIPPED
    filetypes_to_gzip = ('js', 'css', 'html', 'jpg', 'ico')

    print( '\nGZIP: INITIATED GZIP FOR SPIFFS...\n' )
    data_dir_path = os.path.join(env.get('PROJECTSRC_DIR'), "WebServer", "data")
    print(data_dir_path)
    gzip_dir_path = os.path.join(env.get('PROJECTDATA_DIR'))

    # CHECK DATA DIR
    if not os.path.exists(data_dir_path):
        print('GZIP: DATA DIRECTORY MISSING AT PATH:' + data_dir_path)
        print('GZIP: PLEASE CREATE THE DIRECTORY FIRST (ABORTING)')
        print('GZIP: FAILURE / ABORTED')
        return

    # CHECK GZIP DIR
    if not os.path.exists(gzip_dir_path):
        print('GZIP: GZIP DIRECTORY MISSING AT PATH: ' + gzip_dir_path)
        print('GZIP: TRYING TO CREATE IT...')
        try:
            os.mkdir(gzip_dir_path)
        except Exception as e:
            print('GZIP: FAILED TO CREATE DIRECTORY: ' + gzip_dir_path)
            # print( 'GZIP: EXCEPTION... ' + str( e ) )
            print('GZIP: PLEASE CREATE THE DIRECTORY FIRST (ABORTING)')
            print('GZIP: FAILURE / ABORTED')
            return

    # DETERMINE FILES TO COMPRESS
    files_to_gzip = []
    gzipped_dirnames = []

    for (dirpath, dirnames, filenames) in os.walk(data_dir_path):
        files_to_gzip += [os.path.join(dirpath, file) for file in filenames if file.endswith(filetypes_to_gzip)]
        if (len(dirnames) > 0):
            # LU_TODO, only single directory depth needed for now
            gzipped_dirnames = dirnames
    print(gzipped_dirnames)

    print('GZIP: CREATING GZIPPED FOLDERS IN PROJECTDATA_DIR... {}\n'.format(gzipped_dirnames))

    try:
        for d in gzipped_dirnames:
            dir_path = os.path.join(gzip_dir_path, d)
            print('GZIP: CREATING DIR: ' + dir_path)
            # CHECK IF FILE ALREADY EXISTS
            if os.path.exists(dir_path):
                print('GZIP: REMOVING... ' + dir_path)
                os.remove(dir_path)
            os.mkdir(dir_path)
    except IOError as e:
        print('GZIP: EXCEPTION... {}'.format(e))


    print('GZIP: GZIPPING FILES... {}\n'.format(files_to_gzip))
    # COMPRESS AND MOVE FILES
    was_error = False
    try:
        for source_file_path in files_to_gzip:
            print('GZIP: ZIPPING... ' + source_file_path)
            splitted = os.path.normpath(source_file_path).split(os.path.sep)
            if (splitted[-2] == 'data'):
                tmp_path = splitted[-1] +'.gz'
            else:
                tmp_path = os.path.join(splitted[-2], splitted[-1] + '.gz')
            target_file_path = os.path.join(gzip_dir_path, tmp_path)
            # CHECK IF FILE ALREADY EXISTS
            if os.path.exists(target_file_path):
                print('GZIP: REMOVING... ' + target_file_path)
                os.remove(target_file_path)

            # print( 'GZIP: GZIPPING FILE...\n' + source_file_path + ' TO...\n' + target_file_path + "\n\n" )
            print('GZIP: GZIPPED... ' + target_file_path + "\n")
            gzip_file(source_file_path, target_file_path)
    except IOError as e:
        was_error = True
        print('GZIP: FAILED TO COMPRESS FILE: ' + source_file_path)
        # print( 'GZIP: EXCEPTION... {}'.format( e ) )
    if was_error:
        print('GZIP: FAILURE/INCOMPLETE.\n')
    else:
        print('GZIP: SUCCESS/COMPRESSED.\n')

# IMPORTANT, this needs to be added to call the routine
env.AddPreAction( '$BUILD_DIR/spiffs.bin', gzip_webfiles )