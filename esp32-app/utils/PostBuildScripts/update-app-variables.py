
Import('env', 'projenv')

import os
from email.utils import formatdate
import fileinput


def http_date(epoch_seconds=None):
    """
    Format the time to match the RFC1123 date format as specified by HTTP
    RFC7231 section 7.1.1.1.
    `epoch_seconds` is a floating point number expressed in seconds since the
    epoch, in UTC - such as that outputted by time.time(). If set to None, it
    defaults to the current time.
    Output a string in the format 'Wdy, DD Mon YYYY HH:MM:SS GMT'.
    Taken from Django: https://github.com/django/django/blob/master/django/utils/http.py#L142
    """
    return formatdate(usegmt=True)


def update_app_vars(source, target, env):
    app_header_file_path = os.path.join(env.get('PROJECT_SRC_DIR'), "App", "app_vars.h")
    print("APP_UPDATE: ",app_header_file_path)
    print(http_date())
    for line in fileinput.input(app_header_file_path, inplace=True):
        if "APP_DATE_LAST_MODIFIED" in str(line):
            new_line = '#define APP_DATE_LAST_MODIFIED "{}"'.format(http_date())
            print(new_line, end='')
        else:
            print(line, end='')



# IMPORTANT, this needs to be added to call the routine
env.AddPreAction('$BUILD_DIR/spiffs.bin', update_app_vars)