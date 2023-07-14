import os
import shutil

# Inserisci il percorso della directory principale qui
main_dir = './'

for dir_name in os.listdir(main_dir):
    if os.path.isdir(os.path.join(main_dir, dir_name)) and not dir_name.startswith('.'):
        # Dividi il nome della directory in base al carattere '-'
        split_dir_name = dir_name.split('-')
        # Prendi il primo carattere del primo elemento della lista
        n = split_dir_name[0][0]
        # Prendi il resto del primo elemento e il secondo elemento della lista
        params = '-'.join(split_dir_name[1:])
        
        # # Elimina tutti i file .ipynb nella sottodirectory
        # for file_name in os.listdir(os.path.join(main_dir, dir_name)):
        #     if file_name.endswith('.ipynb'):
        #         os.remove(os.path.join(main_dir, dir_name, file_name))
        
        # # Copia i file nelle sottodirectory
        # for file_name in ['Analisi_RUN.ipynb', 'Analisi_VEC.ipynb']:
        #     new_file_name = f"{n}_{file_name[:-6]}-{params}.ipynb"
        #     shutil.copy(os.path.join(main_dir, file_name), os.path.join(main_dir, dir_name, new_file_name))


         # Crea la nuova sottodirectory per le immagini
        img_dir = f"{n}_img-{params}"
        os.makedirs(os.path.join(main_dir, img_dir), exist_ok=True)
        
        # Rinomina tutti i file png nella sottodirectory
        for file_name in os.listdir(os.path.join(main_dir, dir_name)):
            if file_name.endswith('.png') and not (file_name.startswith('1_') or file_name.startswith('2_')):
                new_png_name = f"{n}_{file_name[:-4].replace(' ', '_')}-{params}.png"
                shutil.move(os.path.join(main_dir, dir_name, file_name), os.path.join(main_dir, img_dir, new_png_name))
