# Solar System 3D - OpenGL Project

## About the Project

**Solar System 3D** is an interactive application that simulates the solar system in 3D using OpenGL technology. The main goal of the project is to offer a visually attractive representation of the solar system, allowing the user to explore the relative positions of the planets, their orbits, and interactions. Initially conceived as a study in computer graphics, the project has evolved into an educational and visually engaging tool.

## Folders

The repository consists of two folders, "pt-br" and "en-us." Both contain the same project with the same functionalities and the same final result. However, the code is in the language of the folders, with "pt-br" being Brazilian Portuguese and "en-us" being American English. This makes it much easier to interpret the code in your native language without the need for translation and potential errors during the process. You're welcome!

---

## Objectives

- Create an interactive and visually appealing representation of the solar system in 3D.
- Explore the concepts of 3D graphics, lighting, and user interaction.
- Provide an educational experience to learn about the solar system.

## History

**Solar System 3D** originated as a final project for the Computer Graphics course. The initial idea was to create an application that only showed the Earth orbiting the sun. However, during development, the proposal expanded and matured, evolving from a simple planetary representation to a complete simulation of the solar system.

Before arriving at the idea of the solar system, other proposals were considered, including a replication of the classic Atari game "Space Invaders" and a 3D visualization of various fruits, with a focus on an orange. It was from this last idea that the inspiration to create a simulation of the solar system arose, leveraging the educational and learning potential of the application.

---

## Requirements

Before running **Solar System 3D**, make sure to meet the following requirements:

- **Visual Studio Platform (from 2019 onwards)**: The project was developed and tested using Visual Studio Code. Make sure you have the latest version installed.

- **OpenGL-Linked Libraries (GL Folder)**: The project uses OpenGL technology for 3D rendering, and almost the entire project relies on the files in the GL folder.

- **Windows Library**: Since the project involves window resources and OpenGL, it's important to have the relevant Windows libraries to ensure proper execution.

Additionally, you need to have the "Desktop Development with C++" and "Universal Windows Platform Development" extensions installed in Visual Studio.

Ensure that you meet these requirements to ensure a smooth experience when running the **Solar System 3D** project.

---

## How It Works?

To run the project, follow the steps below:

1. **Check the GL Folder**: Ensure that the `GL` folder is present in the project directory. This folder contains the necessary libraries for the application to work. 

2. **Install the GL Folder**: With Visual Studio open, locate `main.cpp` in the "Source Files" and go into the properties of `main.cpp` (`Alt+Enter`). Then, go to:

    ```Configuration Properties > General > C/C++ > General > Additional Include Directories```

    Now, click on the arrow and click edit. Once the new "Additional Include Directories" window opens, click on the yellow folder icon with a little shine to add a new line (`Alt + Insert`). Then, enter the path of the GL folder in the new line.

    *Example: "D:\Code\Visual Studio\Final Project Computer Graphics - 3rd Semester\pt-br\Project\GL;%(AdditionalIncludeDirectories)"*

    Then, click ```OK > Apply``` to finish this step.

3. **Copy and Paste files**: Before you compile and finally run the program, it's important to make sure that all the files in the `GL` folder also exists in the project folder. If you can't see them in the System Solar folder, simply copy and paste the files from the `GL` folder to the Solar System folder. 
 
4. **Compile and Run**: After completing the above three steps, you need to compile the program and run it. You can do it however you prefer, either through commands or using local debugging in Windows through Visual Studio. Both options will work, but make sure you are doing it in the x86 version.

5. **Visualization**: The application allows the user to view the solar system in action. There is no direct interaction with the elements of the simulation. The rotation of the planets and the moon's orbit are controlled automatically.

If you encounter difficulties with installation or execution, feel free to comment about it or directly contact me via Github email for support.

---

## Additional Information

- **Future Enhancements**: The solar system simulation is an ongoing project and may receive updates in the future, including improvements in planetary rotation and possible additional features.

- **Notes**: Some parts of the application may seem complex or may be implemented differently. This project was an opportunity for learning and practice, and I am committed to continuously improving the code.

---

## Acknowledgments

I would like to express my gratitude to all the people who contributed to the development of this project:

- To my friends and colleagues who provided valuable feedback and constant support.

- To my teachers who shared their knowledge and guidance throughout the process.

- To the open-source community for providing essential tools and resources for development.

Thank you to everyone who made this project possible and enriching!

---

This project is an educational and learning initiative. Feel free to explore, modify, and expand it according to your curiosity and needs. Enjoy exploring the solar system in a new dimension!
