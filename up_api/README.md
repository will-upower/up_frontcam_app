# UP DEMO APP API Documentation
<a name="readme-top"></a>
<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">

<h3 align="center">UP OpenCL API Documentation</h3>

  <p align="center">
    The API aims to develop a wrapped OpenCL shared library that incorporates various software Image Signal Processing (ISP) functions essential for image enhancement, manipulation, and correction. These functions include color conversion, cropping, distortion correction, and more. The wrapped library will be integrated into our U Power Front Camera Demo application to showcase its capabilities in real-time image processing.
    <br />
    <a href="https://github.com/will-upower/up_frontcam_app/tree/main/documentation"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/will-upower/up_frontcam_app/blob/main/frontcam_with_cdnn_ref_app_v4h2">View Demo</a>
    ·
    <a href="https://github.com/will-upower/up_frontcam_app/issues/">Report Bug</a>
    ·
    <a href="https://github.com/will-upower/up_frontcam_app/issues/">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li><a href="#background">Background</a></li>
    <li>     
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
The project will deliver a wrapped OpenCL shared library equipped with essential software ISP functions for image enhancement and manipulation. Additionally, a demo application will showcase the capabilities of the library through real-time image processing tasks. The project aims to provide a versatile solution for developers seeking to incorporate advanced image processing capabilities into their applications leveraging GPU acceleration.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Built With

[OpenCL](https://www.khronos.org/opencl/)                      
[C/C++](https://cplusplus.com/)            
[ShellScript](https://www.shellscript.sh/)            

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- Background -->
## Background
### Camera Information
```
Camera Resolution: 1920x1020
Pixel Format:  YUYV422 (10-bit)
Color Code: Y210
Optical Information: 60/200 degrees FOV
```
### NN model
There are currently only two formats we will concern ourselves with                                 
```
RGB 1024x1024
RGB  640x640
```
<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started
### Installation

1. First update your system.
```
sudo apt update
sudo apt upgrade
```
2. Install related packages
```
sudo apt install build-essential -y
sudo apt install cmake -y
```
3. Install OpenCL
```
sudo apt install opencl-headers ocl-icd-opencl-dev -y
```
<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

Run `compile_on_target.sh` on target device (e.g. V4H), and then build the project on a x86_64 device by running `1_build_frontcam_ref_app.sh`.           
And then, scp and run the demo binary `frontcam_with_cdnn_ref_app_v4h2` on target.

_For more examples, please refer to the [Documentation](https://github.com/will-upower/up_frontcam_app/tree/add_readme/documentation)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- LICENSE -->
## License

Distributed under the U Power USA License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

Lawrence Xu - lawrence.xu@upower.com

Project Link: [https://github.com/will-upower/up_frontcam_app](https://github.com/will-upower/up_frontcam_app)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* []()
* []()
* []()

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/will-upower/up_frontcam_app.svg?style=for-the-badge
[contributors-url]: https://github.com/will-upower/up_frontcam_app/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/will-upower/up_frontcam_app.svg?style=for-the-badge
[forks-url]: https://github.com/will-upower/up_frontcam_app/network/members
[stars-shield]: https://img.shields.io/github/stars/will-upower/up_frontcam_app.svg?style=for-the-badge
[stars-url]: https://github.com/will-upower/up_frontcam_app/stargazers
[issues-shield]: https://img.shields.io/github/issues/will-upower/up_frontcam_app.svg?style=for-the-badge
[issues-url]: https://github.com/will-upower/up_frontcam_app/issues
[license-shield]: https://img.shields.io/github/license/will-upower/up_frontcam_app.svg?style=for-the-badge
[license-url]: https://github.com/will-upower/up_frontcam_app/blob/master/LICENSE.txt
