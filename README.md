<p align="center">
  <img src="readme_assets/logo.png">
</p>

---

<p align="center">
  <img src="readme_assets/slide_2_small.png">
  <img src="readme_assets/slide_3_small.png">
  <img src="readme_assets/slide_4_small.png">
  <img src="readme_assets/slide_5_small.png">
  <img src="readme_assets/slide_6_small.png">
</p>

---

**micro:net** is a classroom activity that aims to introduce students to some interesting concepts in computer networks in a hands-on way. Rather than static diagrams of a network or web-based simulations, students can play around with a *physical* network of micro:bits scattered around the room. By adding, removing or just moving micro:bits and seeing the messages exchanged between them, students can get a sense of the simple rules that can make a collection of dumb micro:bits operate as a dynamic network.

The activity doesn't assume any particular year group and can be useful for covering things like what a network is to more morer advanced topics like [Link State Routing](https://en.wikipedia.org/wiki/Link-state_routing_protocol) which we've used to implement micro:net.

*Video and some lesson plan ideas coming soon!*

# Running It

You'll need:
* **micro:bits** - how many depends on the size of your classroom, we recommend starting with at least 4.
* **PCs** - this lets you run the desktop app and see what the connected micro:bit can see. Just using one connected to a big monitor or projector works but it's great to have more so you can compare what the network looks like at different points.
* **Battery Packs** - something like [this](https://www.amazon.co.uk/2xAAA-Battery-Cage-Connector-microbit/dp/B01BSZYJUW). These let you move around micro:bits more easily so you can try out different network shapes.

Getting setup:
1. Install the software on the PCs using the instructions below.
2. With the dektop application open, connect a micro:bit to the PC via a USB cable before clicking 'Flash micro:bit' and selecting the folder for the connected micro:bit. You'll just need to do this once for every micro:bit you'll be using.

After that, just follow the rest of the instructions given in the slides above (or under 'help' in the desktop app) to start your micro:net.

## Installation

**[Latest releases here!](https://github.com/Microboys/micronet/releases/latest)**

### Windows
Download the `.exe` installer from the [latest releases](https://github.com/Microboys/micronet/releases/latest) and run it.

**Important:** if you're on Windows 10, you might need to update a driver so you can talk to the micro:bit - check out [this guide](https://support.microbit.org/support/solutions/articles/19000022103-how-do-i-use-the-serial-port-with-a-micro-bit-on-windows). This will require admin rights.

### MacOS
Download the `.app` from the [latest releases](https://github.com/Microboys/micronet/releases/latest) and run it.

### Linux

Download and run the file ending in `.AppImage` from the [latest releases](https://github.com/Microboys/micronet/releases/latest).

# Issues

We're doing our best to keep things robust but we're aware of a few bugs. Resetting the micro:bit (pressing the little black button) or restarting the desktop application often fixes the problem but if you're still having problems and its not in the known issues below, please let us know.

* **Micro:net crashes when there are more than 15 connections.** This is because we're already using up quite a lot of the micro:bit's memory and can't find any more space. If this happens, try moving the micro:bits further apart to reduce the number of connections and restarting them.
* **Duplicate events in the desktop app.** We're looking into this one. For now, restarting the desktop app should fix it.
* **Desktop app doesn't connect to the micro:bit anymore.** If restarting the application _and_ resetting the micro:bit doesn't fix it, make sure you can see the micro:bit folder on your PC and that you're pressing the 'activate' button on the micro:bit to connect it. If you're still not getting anywhere, please let us know.
