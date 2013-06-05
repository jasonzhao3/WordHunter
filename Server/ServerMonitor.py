##########################################
# Author: Yang Zhao (yzhao3@stanford.edu) 
# Mentor: David Chen, Sam Tsai 
##########################################

import os
import string
import time

print('Server monitor program starts!')
while (True):
	# % Wait until image is ready
    imageReadyFile = './upload/image_ready';
    if not os.path.exists(imageReadyFile): 
        #time.sleep(0.1);
        #print('Waiting for image-is-ready signal');
        continue;
    
    # % Read input image file
    fid = open(imageReadyFile, 'r');
    imageFile = fid.read();
    root, ext = os.path.splitext(imageFile);
    fid.close();
    
    # get pathname, filename, wordToSearch
    lastSlash = root.rfind("/");
    pathStr = root[:lastSlash];
    name = root[lastSlash+1 :];
    wordToSearch = ext.split('jpg')[1];
    #print("the word to search is : " + wordToSearch);
    #print('Processing image: ' +  imageFile);
    
    # % Remove image-is-ready file
    os.remove(imageReadyFile);
    
    inputImageFile = './upload/' + name + '.jpg';
    # call extract_text executable  
    cmd = './extract_text/extract_text ' + inputImageFile + ' ' + wordToSearch;
    os.system(cmd);
    
    # % Signal that result is ready
    resultReadyFile = './output/result_ready';
    fid = open(resultReadyFile, 'w');
    # we can write the output coordinate into resultReadyFile
    
    fid.write('1');
    fid.close();
