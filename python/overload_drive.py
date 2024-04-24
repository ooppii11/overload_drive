import os
import string
import random
import shutil
from multiprocessing import Pool, cpu_count

GB = 1000000000
MB = 1000000

def delete_drive_contents(drive):
    for filename in os.listdir(drive):
        file_path = os.path.join(drive, filename)
        try:
            if os.path.isfile(file_path) or os.path.islink(file_path):
                #os.unlink(file_path)
                os.remove(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)
        except Exception as e:
            print(f'Failed to delete {file_path}. Reason: {e}')

    print(f"Drive '{drive}' cleared.")


def create_random_file(file_name, size):
    with open(file_name, 'w') as file:
        random_data = ""
        if(size > len("nice try")):
            random_data = "nice try\n"
            file.write(random_data)
            size = size - len("nice try")

        for _ in range(size - size//MB):
            random_data += random.choice(string.ascii_letters + string.digits + string.punctuation + string.whitespace + string.hexdigits)
            if(len(random_data) == MB):
                random_data += "\n"
                file.write(random_data)
                random_data = ""

        file.write(random_data)

        print(f"File '{file_name}' created with {size} bytes of random data.")


def overload_drive_one_file(drive):
    total, used, free = shutil.disk_usage(drive)

    print(f"Drive '{drive}' has {free} bytes of free space.")
    file_name = f'{drive}random_file.txt'
    create_random_file(file_name, free)


def overload_drive_multi_files(drive, size=GB):
    if(size < GB):
        print("Minimum offset file size is 1 GB.")
        size = GB

    total, used, free = shutil.disk_usage(drive)

    num_files = free // size

    if(num_files < 1):
        size = free
        num_files = 1
    

    print(f"Drive '{drive}' has {free} bytes of free space.")
    print(f"Creating {num_files} files...")

    files_names = [f"{drive}random_file_{n+1}.txt" for n in range(num_files)]
    with Pool(processes=cpu_count()) as p:
        p.starmap(create_random_file, [(file_name, size) for file_name in files_names])

    p.close()
    p.join()
    
    overload_drive_one_file(drive)


def main():
    try:
        drive_letter = input("Enter the drive letter (e.g., C:): ").upper()
        if not os.path.exists(f"{drive_letter}:\\"):
            print("Invalid drive letter.")
            return

        multi_or_one = input("Overload with one file or multiple files? (O or M): ").upper()
        if multi_or_one == "O":
            overload_drive = overload_drive_one_file
        elif multi_or_one == "M":
            overload_drive = overload_drive_multi_files
        else:
            print("Invalid choice.")
            return
            
        drive_path = f"{drive_letter}:\\"

        delete_drive_contents(drive_path)
        overload_drive(drive_path)
        print("Drive overloaded successfully.")
        delete_drive_contents(drive_path)
    
        
    except KeyboardInterrupt:
        print("\nExiting...")


if __name__ == "__main__":
    main()