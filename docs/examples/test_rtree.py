#compas_wood
from compas_wood.joinery import test
from compas_wood.joinery import rtree
from compas_wood.data.joinery_solver_data_set import *

#viewer
from compas_wood.viewer_helpers import display

# ==============================================================================
# Create a list of polyline pairs - input, then generate joints and display them
# ==============================================================================

def test_rtree():


    # Get a list of polyline pairs
    input = ss_0()

    #Compute Rtree
    neighbours, boxes_AABB, boxes_OOBB = rtree(input)

    selected_id = 22
    boxes_selected = [] 
    boxes_AABB_or_boxes_OOBB = False

    if(boxes_AABB_or_boxes_OOBB):
        boxes_selected.append(boxes_OOBB[selected_id])
        for i in neighbours[selected_id]:
            for j in i : 
                boxes_selected.append(boxes_OOBB[j])
    else :
        boxes_selected.append(boxes_AABB[selected_id])
        for i in neighbours[selected_id]:
            for j in i : 
                boxes_selected.append(boxes_AABB[j])

    # Display via Compas_View2
    display(input, None,boxes_OOBB,0.01,0,0,0,False)


    #output
    return neighbours

# ==============================================================================
#call the compas_wood methods
# ==============================================================================
test_rtree()