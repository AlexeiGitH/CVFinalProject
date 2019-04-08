Instructions for image annotation:

**You will need python installed on your computer, and possibly other libraries like dlib**

* -d is directory where the images are
* -a annotations output file name
* -i images output file name

Command example:
`python gather_annotations.py -d Train/ -a annot.npy -i images.npy`

![Terminal](https://i.imgur.com/LTxBJlf.png) 

in this example Train/ is a directory where the images are
  - annot.npy - is a file where the results are saved
  - images.npy - is a file where image paths are saved 

Click space to move to the next picture.

You cannot undo actions and move back

![alt text](https://i.imgur.com/rzXGmZH.jpg)


**Keep the ratio of all rectangles, try not to have squares**
![alt text](https://i.imgur.com/gb3fUTh.png)
