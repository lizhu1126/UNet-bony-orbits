# UNet-bony-orbits
Exploring the aging of bony orbits based on an segmentation network

# 1.BackGround
This code is used to realize the automatic segmentation of the orbital region of the facial and skull image, and then realize the automatic measurement of the area and height of the segmented bony orbit. The craniofacial image dataset should be prepared of skull CT scanning data in DICOM format. You can use this code to quickly verify larger samples.

# 2.Requirements
- Ubuntu18.04
- python 3.8
- pytorch 1.7
- cuda 11.0
- anaconda 4(recommended to use)
- OpenCV (4.0+)

# 3.Usage
## 3-1.segmentation
The code for segmentation is in the folder of "0 segmentation".
You can find out how to use it in this repository[UNet++](https://github.com/4uiiurz1/pytorch-nested-unet)

## 3-2.automatic measurement of the area and height of the segmented bony orbit
The code for segmentation is in the folder of "1 calculation".
Use this code you can release the automatic measurement of the area and height of the segmented bony orbit.
