
Import('env', 'projenv')

import os
from datetime import datetime
from dateutil.parser import parse as parsedate

def update_app_vars(source, target, env):
    app_header_file_path = os.path.join(env.get('PROJECTSRC_DIR'), "App", "app.h")
    print("APP_UPDATE: ",app_header_file_path)
    print(datetime.now())


# IMPORTANT, this needs to be added to call the routine
env.AddPreAction('$BUILD_DIR/spiffs.bin', update_app_vars)