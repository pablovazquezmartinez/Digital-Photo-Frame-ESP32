import os #we import the extension
from PIL import Image #we import the extension
from pillow_heif import register_heif_opener #this is to be able to work with the .heic and .heif images (iphone format)
register_heif_opener() #activate the extension to use the .heic images


try:
    os.chdir(str(input("Introduce the full location of the folder with the images you want to convert: "))) #choosing the folder
    os.makedirs('Images-ready-to-use', exist_ok=True) #creating a new folder inside the folder chosen
    for filename in os.listdir('.'):
        if not filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.heic', '.heif')): #making sure we use the code and add to the new folder only images supported by pillow
            continue
        IMG = Image.open(filename) #open the image to work with
        Formatname = str.lower(IMG.format) #saving the original format of the image before any change to be able to add something to the name at the end
        print('Converting %s...' % (filename)) #for cheching the progress
        IMG = IMG.resize((320, 240)) #change the format to 320x240
        IMG = IMG.convert('RGB') #convert the image to RBG (24 bits)
        initial_name, _ = os.path.splitext(filename) #we separate the filename with the extension
        output_path = os.path.join('Images-ready-to-use', f'{initial_name}--ready.bmp') #we create the output full path, inside the folder created and add "ready" at the end
        IMG.save(output_path, format='BMP')
    print("IMAGES READY.")

except Image.UnidentifiedImageError: #In case one image is corrupted
    print("The image " + str(filename) + " is corrupted. Please, check.")

except OSError: #In case the full location of the folder is incorrect
    print("The location of the folder is incorrect. Please, check.")
