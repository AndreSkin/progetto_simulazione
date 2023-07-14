import os
import shutil

path = './'
for filename in os.listdir(path):
    if filename.startswith('General-'):
        params = filename.split('-')[1].split('#')[0]
        new_folder = os.path.join(path, '2_results-' + params)
        if not os.path.exists(new_folder):
            os.makedirs(new_folder)
        shutil.move(os.path.join(path, filename), new_folder)