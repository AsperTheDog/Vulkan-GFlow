# GFlow
GFLow is a **work in progress** Vulkan learning tool. The main objective is to abstract concepts like Render Passes, Graphics Pipelines and Synchronization into a visually intuitive graph based system.
The users will be able to preview the results of their rendering configuration as they create it and receive detailed explanations on each step, curated to their specific case as much as possible. The resulting low-level configuration will be visualized on demand by the app.
Additionally, the application is designed to have each layer of abstraction as independent as possible, enabling the use of each one independently from each other. This means that:
- When you just want to execute an exported project you can simply use the core library
- When you want to create a project and export it through an API (i.e. an automated system or a homebrew editor) you can simply use the parser library
