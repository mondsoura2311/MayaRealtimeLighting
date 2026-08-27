# Maya Real-time Lighting Plugin - Example Scene File
# This is a basic Maya ASCII scene file (.ma) template
# Can be opened directly in Maya

requires maya "2020";

file -f -new;
currentUnit -l centimeter -a degree -t film;

// Create a simple polygon sphere
setAttr ".v" no;
setAttr ".vir" yes;
setAttr ".vif" yes;
setAttr ".kod" yes;

// Default camera setup
camera -position 0 3 15 -rotation -10 0 0;

// Lighting setup would be added here
// This is a stub for the example scene file
