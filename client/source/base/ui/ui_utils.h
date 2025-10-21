#pragma once


namespace Utils
{

/**
 * Recursively scale gameplay::Control and all its children by some factors.
 * The margin, padding and border are scaled as well.
 */
void scaleUIControl(gameplay::Control* control, float kx, float ky);



/**
 * Calculate total width and height of all visible children in the container.
 * This matches scrollable width/height.
 */
void measureChildrenBounds(gameplay::Container* container, float* width, float* height);

}