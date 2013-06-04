import os
import string
import time

while (True):
	# % Wait until image is ready
    imageReadyFile = './upload/image_ready';
    if not os.path.exists(imageReadyFile): 
        time.sleep(0.5);
        #print('Waiting for image-is-ready signal');
        continue;
    
    # % Read input image file
    fid = open(imageReadyFile, 'r');
    imageFile = fid.read();
    root, ext = os.path.splitext(imageFile);
    fid.close();
    
    lastSlash = root.rfind("/");
    pathStr = root[:lastSlash];
    name = root[lastSlash+1 :];

    wordToSearch = ext.split('?')[1];
    #mode = ext.split('?')[2];
    print("the word to search is : " + wordToSearch);
    #print("mode is : " + mode);


    # [pathStr, name, ext] = fileparts(imageFile);
    print('Processing image: ' +  imageFile);
    
    # % Remove image-is-ready file
    os.remove(imageReadyFile);
    
    # # % Call SIFT keypoint extractor
    # if ext(1) ~= '.'
    #     ext = ['.' ext];
    # end

    inputImageFile = './upload/' + name + '.jpg';
    outputImageFile = './output/processed_' + name + '.jpg';
# %     
# %     img = imread(inputImageFile);
# %     img_histogram = imhist(img);
# %     imwrite(img_histogram, outputImageFile);

    cmd = './extract_text ' + inputImageFile + ' ' + wordToSearch;
    os.system(cmd);
    
    # % Signal that result is ready
    resultReadyFile = './output/result_ready';
    fid = open(resultReadyFile, 'w');
    # we can write the output coordinate into resultReadyFile
    
    fid.write('1');
    fid.close();
