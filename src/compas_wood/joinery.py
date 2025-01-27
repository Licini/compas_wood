import wood_pybind11
from wood_pybind11 import WoodVectorInt
from wood_pybind11 import WoodNestedVectorInt
from wood_pybind11 import WoodVectorDouble
from wood_pybind11 import WoodNestedVectorDouble
from wood_pybind11 import WoodNestedNestedVectorDouble
from wood_pybind11 import WoodVectorFloat
from compas.geometry import Polyline
from compas.datastructures import Mesh
from compas.geometry import Point
from compas.geometry import Frame
from compas.geometry import Box
import time



# from wood_pybind11 import test
# from wood_pybind11 import read_xml_polylines
# from wood_pybind11 import read_xml_polylines_and_properties
# from wood_pybind11 import get_connection_zones

# existing data-sets
filenames_of_datasets = [
    "type_beams_name_phanomema_node",  # 0
    "type_plates_name_cross_and_sides_corner",  # 1
    "type_plates_name_cross_corners",  # 2
    "type_plates_name_cross_ibois_pavilion",  # 3
    "type_plates_name_cross_square_reciprocal_iseya",  # 4
    "type_plates_name_cross_square_reciprocal_two_sides",  # 5
    "type_plates_name_cross_vda_corner",  # 6
    "type_plates_name_cross_vda_hexshell",  # 7
    "type_plates_name_cross_vda_hexshell_reciprocal",  # 8
    "type_plates_name_cross_vda_shell",  # 9
    "type_plates_name_cross_vda_single_arch",  # 10
    "type_plates_name_hexbox_and_corner",  # 11
    "type_plates_name_joint_linking_vidychapel_corner",  # 12
    "type_plates_name_joint_linking_vidychapel_full",  # 13
    "type_plates_name_joint_linking_vidychapel_one_axis_two_layers",  # 14
    "type_plates_name_joint_linking_vidychapel_one_layer",  # 15
    "type_plates_name_side_to_side_edge_inplane_2_butterflies",  # 16
    "type_plates_name_side_to_side_edge_inplane_differentdirections",  # 17
    "type_plates_name_side_to_side_edge_inplane_hexshell",  # 18
    "type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners",  # 19
    "type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_combined",  # 20
    "type_plates_name_side_to_side_edge_inplane_outofplane_simple_corners_different_lengths",  # 21
    "type_plates_name_side_to_side_edge_outofplane_box",  # 22
    "type_plates_name_side_to_side_edge_outofplane_dodecahedron",  # 23
    "type_plates_name_side_to_side_edge_outofplane_folding",  # 24
    "type_plates_name_side_to_side_edge_outofplane_icosahedron",  # 25
    "type_plates_name_side_to_side_edge_outofplane_inplane_and_top_to_top_hexboxes",  # 26
    "type_plates_name_side_to_side_edge_outofplane_octahedron",  # 27
    "type_plates_name_side_to_side_edge_outofplane_tetra",  # 28
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box",  # 29
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_box_pair",  # 30
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_corner",  # 31
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_full_arch",  # 32
    "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small",  # 33
    "type_plates_name_top_to_side_box",  # 34
    "type_plates_name_top_to_side_corners",  # 35
    "type_plates_name_top_to_side_pairs",  # 36
    "type_plates_name_top_to_top_pairs",  # 37
]


def WoodVectorInt_to_list(wood_vector_int):
    my_list = []

    for i in range(0, len(wood_vector_int)):
        my_list.append(wood_vector_int[i])

    return my_list


def list_to_WoodVectorInt(list):
    my_list = WoodVectorInt(list)

    return my_list


def WoodNestedVectorInt_to_lists(wood_nested_vector_int):
    my_lists = []

    for i in range(len(wood_nested_vector_int)):
        my_list = []
        for j in range(0, len(wood_nested_vector_int[i])):
            my_list.append(wood_nested_vector_int[i][j])
        my_lists.append(my_list)

    return my_lists


def lists_to_WoodNestedVectorInt(lists):
    my_lists = WoodNestedVectorInt()

    for i in lists:
        my_lists.append(WoodVectorInt(i))

    return my_lists


def WoodVectorDouble_to_list(wood_vector_double):
    my_list = []

    for i in range(0, len(wood_vector_double)):
        my_list.append(wood_vector_double[i])

    return my_list


def list_to_WoodVectorDouble(list):
    my_list = WoodVectorDouble(list)

    return my_list


def WoodNestedVectorDouble_to_lists(wood_nested_vector_double):
    my_lists = []

    for i in range(len(wood_nested_vector_double)):
        my_list = []
        for j in range(0, len(wood_nested_vector_double[i])):
            my_list.append(wood_nested_vector_double[i][j])
        my_lists.append(my_list)

    return my_lists


def lists_to_WoodNestedVectorDouble(lists):
    my_lists = WoodNestedVectorDouble([])

    for i in lists:
        my_lists.append(WoodVectorDouble(i))

    return my_lists


def lists_of_vectors_to_WoodNestedVectorDouble(lists):
    my_lists = WoodNestedVectorDouble()

    for i in lists:
        list_coord = []
        for vector in i:
            list_coord.append(vector[0])
            list_coord.append(vector[1])
            list_coord.append(vector[2])
        my_lists.append(WoodVectorDouble(list_coord))

    return my_lists


def WoodNestedVectorDouble_to_polylines(wood_nested_vector_double):
    # polylines
    output_polylines = []

    for i in range(len(wood_nested_vector_double)):
        points = []
        for j in range(0, len(wood_nested_vector_double[i]), 3):
            points.append(
                [
                    wood_nested_vector_double[i][j + 0],
                    wood_nested_vector_double[i][j + 1],
                    wood_nested_vector_double[i][j + 2],
                ]
            )
        polyline = Polyline(points)
        output_polylines.append(polyline)

    return output_polylines


def WoodVectorFloat_and_WoodVectorInt_to_mesh(wood_vector_float, wood_vector_int):
    # >>> vertices_1 = [[0, 0, 0], [0, 500, 0], [500, 500, 0], [500, 0, 0]]
    # >>> vertices_2 = [[500, 0, 0], [500, 500, 0], [1000, 500, 0], [1000, 0, 0]]
    # >>> faces = [[0, 1, 2, 3]]

    vertices = []
    faces = []

    for i in range(0, len(wood_vector_float), 3):
        vertices.append(
            [
                wood_vector_float[i + 0],
                wood_vector_float[i + 1],
                wood_vector_float[i + 2],
            ]
        )

    for i in range(0, len(wood_vector_int), 3):
        faces.append(
            [
                wood_vector_int[i + 0],
                wood_vector_int[i + 1],
                wood_vector_int[i + 2],
            ]
        )

    return Mesh.from_vertices_and_faces(vertices, faces)


def polylines_to_WoodNestedVectorDouble(lists):
    my_lists = WoodNestedVectorDouble()

    for i in range(len(lists)):
        my_list = []
        for j in lists[i]:
            my_list.append(j[0])
            my_list.append(j[1])
            my_list.append(j[2])
        my_lists.append(WoodVectorDouble(my_list))

    return my_lists


def WoodNestedVectorDouble_to_vectorslist(wood_nested_vector_double):
    # vectors
    output_vectors = []
    for i in range(len(wood_nested_vector_double)):
        vectors = []
        for j in range(0, len(wood_nested_vector_double[i]), 3):
            vectors.append(
                [
                    wood_nested_vector_double[i][j + 0],
                    wood_nested_vector_double[i][j + 1],
                    wood_nested_vector_double[i][j + 2],
                ]
            )
        output_vectors.append(vectors)

    return output_vectors


def WoodNestedNestedVectorDouble_to_polylineslists(wood_nested_nested_vector_double):
    # lists of polylines
    output_polylines = []

    for i in wood_nested_nested_vector_double:
        polylines = []

        for j in i:
            points = []
            for k in range(0, len(j), 3):
                points.append([j[k + 0], j[k + 1], j[k + 2]])
            polylines.append(Polyline(points))

        output_polylines.append(polylines)

    return output_polylines


def WoodNestedNestedVectorDouble_to_polylineslist(wood_nested_nested_vector_double):
    # lists of polylines
    output_polylines = []

    for i in wood_nested_nested_vector_double:
        for j in i:
            points = []
            for k in range(0, len(j), 3):
                points.append([j[k + 0], j[k + 1], j[k + 2]])
            output_polylines.append(Polyline(points))

    return output_polylines


def WoodVectorDouble_to_box(wood_vector_double):
    points = []

    for i in range(0, len(wood_vector_double), 3):
        p = Point(wood_vector_double[i], wood_vector_double[i + 1], wood_vector_double[i + 2])
        points.append(p)

    point_center = (points[0] + points[6]) * 0.5

    xaxis = points[0] - points[4]
    yaxis = points[0] - points[3]
    zaxis = points[0] - points[1]
    frame = Frame(point_center, xaxis, yaxis)

    box = Box(frame, xaxis.length, yaxis.length, zaxis.length)
    vertices, faces = box.to_vertices_and_faces()
    mesh_box = Mesh.from_vertices_and_faces(vertices, faces)

    return mesh_box


def WoodNestedVectorDouble_to_boxeslist(wood_nested_vector_double):
    mesh_boxes = []
    for i in wood_nested_vector_double:
        mesh_boxes.append(WoodVectorDouble_to_box(i))

    return mesh_boxes


def test():
    """Test if Wood library is loaded

    Parameters
    ----------

    Returns
    -------
    Prints
        Hello from CPP Wood
    """

    wood_pybind11.test()


def read_xml_polylines(
    foldername, filename_of_dataset="type_plates_name_cross_vda_hexshell_reciprocal"
):
    """read xml data_sets files from foldername and filename_of_dataset
    Parameters
    ----------
    foldername : str
        The name of the folder where all the .xml data-sets are located e.g. "C://IBOIS57//_Code//Software//Python//compas_wood//frontend//src//wood//dataset//".
    filename_of_dataset : str, optional
        The name of the file e.g. "type_plates_name_cross_vda_hexshell_reciprocal" .
    Returns
    -------
    list[Polyline]
        A list of top and bottom polylines
    """

    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################
    polylines_coordinates = WoodNestedVectorDouble(
        []
    )  # fills with double values e.g. WoodVectorDouble([0.5, 1.4, 2.3])

    print(
        "\n___________________________________start_of_WOOD_____________________________________"
    )
    wood_pybind11.read_xml_polylines(
        foldername,
        filename_of_dataset,
        polylines_coordinates,
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________"
    )

    ###################################################################################################
    # convert nested vector of doubles to a list of polylines
    ###################################################################################################

    output_polylines = []

    for i in range(len(polylines_coordinates)):
        points = []
        for j in range(0, len(polylines_coordinates[i]), 3):
            points.append(
                [
                    polylines_coordinates[i][j + 0],
                    polylines_coordinates[i][j + 1],
                    polylines_coordinates[i][j + 2],
                ]
            )
        polyline = Polyline(points)
        output_polylines.append(polyline)

    ###################################################################################################
    # output
    ###################################################################################################
    return output_polylines


def read_xml_polylines_and_properties(
    foldername,
    filename_of_dataset="type_plates_name_cross_vda_hexshell_reciprocal",
):
    """read xml data_sets files from foldername and filename_of_dataset
    Parameters
    ----------
    foldername : str
        The name of the folder where all the .xml data-sets are located e.g. "C://IBOIS57//_Code//Software//Python//compas_wood//frontend//src//wood//dataset//".
    filename_of_dataset : str, optional
        The name of the file e.g. "type_plates_name_cross_vda_hexshell_reciprocal" .
    Returns
    -------
    tuple:
        std::vector<std::vector<double>>& input_polyline_pairs_coord,
        std::vector<std::vector<double>>& input_insertion_vectors_coord
        std::vector<std::vector<int>>& input_JOINTS_TYPES,
        std::vector<std::vector<int>>& input_three_valence_element_indices_and_instruction,
        std::vector<int>& input_adjacency
        A list of top and bottom polylines
    """

    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################
    input_polyline_pairs_coord = WoodNestedVectorDouble([])
    input_insertion_vectors_coord = WoodNestedVectorDouble([])
    input_JOINTS_TYPES = WoodNestedVectorInt([])
    input_three_valence_element_indices_and_instruction = WoodNestedVectorInt([])
    input_adjacency = WoodVectorInt([])

    print(
        "\n___________________________________start_of_WOOD_____________________________________"
    )

    wood_pybind11.read_xml_polylines_and_properties(
        foldername,
        filename_of_dataset,
        input_polyline_pairs_coord,
        input_insertion_vectors_coord,
        input_JOINTS_TYPES,
        input_three_valence_element_indices_and_instruction,
        input_adjacency,
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________"
    )

    ###################################################################################################
    # convert nested vector of doubles to a list of polylines
    ###################################################################################################

    # polylines
    output_polylines = WoodNestedVectorDouble_to_polylines(input_polyline_pairs_coord)

    # vectors
    output_vectors = WoodNestedVectorDouble_to_vectorslist(
        input_insertion_vectors_coord
    )

    # joint_types
    output_joints_types = WoodNestedVectorInt_to_lists(input_JOINTS_TYPES)

    # three_valence
    output_three_valence_element_indices_and_instruction = WoodNestedVectorInt_to_lists(
        input_three_valence_element_indices_and_instruction
    )

    # adjacency
    output_adjacency = WoodVectorInt_to_list(input_adjacency)

    ###################################################################################################
    # output
    ###################################################################################################
    return (
        output_polylines,
        output_vectors,
        output_joints_types,
        output_three_valence_element_indices_and_instruction,
        output_adjacency,
    )


def get_connection_zones(
    input_polyline_pairs,
    input_insertion_vectors=None,
    input_joints_types=None,
    input_three_valence_element_indices_and_instruction=None,
    input_adjacency=None,
    joint_parameters_and_types=None,
    search_type=0,
    scale=[1, 1, 1],
    output_type=4,
    flatten_output=False,
):
    """detect connection zones between two polylines and generate joinery
    Parameters
    ----------
    input_polyline_pairs : list[Polyline]
        list of polylines of top and bottom outlines
    input_insertion_vectors : list[list[Vector],list[Vector],list[Vector]], optional
        list of lists of insertion vectors for each polyline side: top, bottom and sides
    input_joints_types : list[list[int],list[int],list[int]], optional
        list of lists of joint types for each polyline side: top, bottom and sides
    input_three_valence_element_indices_and_instruction : list[list[int],list[int],list[int]], optional
        list of lists of 4 valence elements e.g. [[0],[0,4,1,4],[5,4,6,4]], the first item points to special corner cases: 0 - Annen Case 1 - Vidy Case
    input_adjacency : list[int], optional
        list of adjacency between polylines elements and faces, often sides are ignored e.g. [0,1,-1,-1,5,8,-1,-1]
    joint_parameters_and_types : list[double], optional
        a list of triplets:
        300, 0.5, 3,   // 1-9 ss_e_ip (side-to-side edge in-plane)
        450, 0.64, 15, // 10-19 ss_e_op (side-to-side edge out-of-plane)
        450, 0.5, 20,  // 20-29 ts_e_p (top-to-side edge plane)
        300, 0.5, 30,  // 30-39 cr_c_ip (cross cutting in-plane)
        6, 0.95, 40,   // 40-49 tt_e_p  (top-to-top edge plane)
        300, 0.5, 58,  // 50-59 ss_e_r (side-to-side edge rotated)
        300, 1.0, 60   // 60-69 b (boundary)
    search_type : int, optional
        0 - side-to-side, 1 - cross, 2 - all
    scale : list[double], optional
        a list of 3 doubles for scaling the joint
        for beams you might need bigger cuts
        for plates you might need smaller cuts
    output_type : int, optional
        what_to_expose:
        0 - Plate outlines
        1 - wood::joint lines
        2 - wood::joint areas
        3 - wood::joint areas with bounding box
        4 - wood::joint areas with bounding box and cut types
    Returns
    -------
    list[Polyline]
        A list of top and bottom polylines
    """

    ###################################################################################################
    # convert compas polylines to nested vector of doubles
    ###################################################################################################

    _input_polyline_pairs_coord = WoodNestedVectorDouble()
    _input_insertion_vectors_coord = WoodNestedVectorDouble()
    _input_joints_types = WoodNestedVectorInt()
    _input_three_valence_element_indices_and_instruction = WoodNestedVectorInt()
    _input_adjacency = WoodVectorInt()

    _input_polyline_pairs_coord = polylines_to_WoodNestedVectorDouble(
        input_polyline_pairs
    )  # input_polyline_pairs

    if input_insertion_vectors is not None:
        _input_insertion_vectors_coord = lists_of_vectors_to_WoodNestedVectorDouble(
            input_insertion_vectors
        )  # input_insertion_vectors

    if input_joints_types is not None:
        _input_joints_types = lists_to_WoodNestedVectorInt(
            input_joints_types
        )  # input_joints_types

    if input_three_valence_element_indices_and_instruction is not None:
        _input_three_valence_element_indices_and_instruction = (
            lists_to_WoodNestedVectorInt(
                input_three_valence_element_indices_and_instruction
            )
        )
        # (input_three_valence_element_indices_and_instruction)

    if input_adjacency is not None:
        _input_adjacency = list_to_WoodVectorInt(input_adjacency)  # input_adjacency

    _joint_parameters_and_types = WoodVectorDouble(
        [
            300.0,  # 1-9 ss_e_ip (side-to-side edge in-plane)
            0.5,
            3.0,
            200.0,  # 10-19 ss_e_op (side-to-side edge out-of-plane)
            0.66,
            10.0,
            300.0,  # 20-29 ts_e_p (top-to-side edge plane)
            0.5,
            20.0,
            300.0,  # 30-39 cr_c_ip (cross cutting in-plane)
            0.5,
            30.0,
            6.0,  # 40-49 tt_e_p  (top-to-top edge plane)
            0.95,
            40.0,
            300.0,  # 50-59 ss_e_r (side-to-side edge rotated)
            0.5,
            58.0,
            300.0,  # 60-69 b (boundary)
            1.0,
            60.0,
        ]
    )

    if (
        joint_parameters_and_types is not None
    ):  # if the user has provided joint parameters
        for i in range(len(joint_parameters_and_types)):
            _joint_parameters_and_types[i] = joint_parameters_and_types[i]

    _output_plines = WoodNestedNestedVectorDouble([])
    _output_types = WoodNestedVectorInt([])
    _scale = WoodVectorDouble([1.0, 1.0, 1.0])

    if scale is not None:  # if the user has provided joint parameters
        for i in range(len(scale)):
            _scale[i] = scale[i]

    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################

    print(
        "\n___________________________________start_of_WOOD_____________________________________"
    )
    
    # print(_input_polyline_pairs_coord)
    # print(_input_insertion_vectors_coord)
    # print(_input_joints_types)
    # print(_input_three_valence_element_indices_and_instruction)
    # print(_input_adjacency)
    # print(_joint_parameters_and_types)
    # print(search_type)
    # print(_scale)
    # print(output_type)
    # print(_output_plines)
    # print(_output_types)
    
    start_time = time.time()
    wood_pybind11.get_connection_zones(
        # input
        _input_polyline_pairs_coord,
        _input_insertion_vectors_coord,
        _input_joints_types,
        _input_three_valence_element_indices_and_instruction,
        _input_adjacency,
        _joint_parameters_and_types,
        search_type,
        _scale,
        output_type,
        # output
        _output_plines,
        _output_types,
    )

    print(
        "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________"
    )

    ###################################################################################################
    # convert nested vector of doubles to a list of polylines
    ###################################################################################################

    ###################################################################################################
    # output
    ###################################################################################################
    if flatten_output:
        return WoodNestedNestedVectorDouble_to_polylineslist(_output_plines)
    else:
        return WoodNestedNestedVectorDouble_to_polylineslists(_output_plines)


def closed_mesh_from_polylines(list_of_polylines):
    """Convert a list of polylines to a closed mesh.

    Parameters
    ----------
    list_of_polylines : list[Polyline]
        list of polylines with holes, for meshing plates

    Returns
    -------
    Mesh
        A closed mesh for top and bottom polylines
    """

    ###################################################################################################
    # conversion of input data
    ###################################################################################################
    list_of_polylines_coordinates = polylines_to_WoodNestedVectorDouble(
        list_of_polylines
    )

    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################

    print(
        "\n___________________________________start_of_WOOD_____________________________________"
    )

    start_time = time.time()
    out_vertices = WoodVectorFloat()
    out_normals = WoodVectorFloat()
    out_triangles = WoodVectorInt()

    wood_pybind11.closed_mesh_from_polylines_vnf(
        # input
        list_of_polylines_coordinates,
        # output
        out_vertices,
        out_normals,
        out_triangles,
    )

    print(
        "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________"
    )

    ###################################################################################################
    # conversion of output data
    ###################################################################################################
    return WoodVectorFloat_and_WoodVectorInt_to_mesh(out_vertices, out_triangles)


def joints(list_of_polylines, search_type=0):
    """Get joints as: a) element pairs, b) joint areas, c) joint types.

    Parameters
    ----------
    list_of_polylines : list[Polyline]
        list of polylines that represents the timber elements

    search_type : int
        0: side-to-side, 1: cross, 2: both

    Returns
    -------
    tuple:
        list[list[int]] element_pairs,
        list[Polyline] joint_areas
        list[int] joint_types
    """

    ###################################################################################################
    # conversion of input data
    ###################################################################################################
    list_of_polylines_coordinates = polylines_to_WoodNestedVectorDouble(
        list_of_polylines
    )
    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################

    print(
        "\n___________________________________start_of_WOOD_____________________________________"
    )
    start_time = time.time()

    element_pairs = WoodNestedVectorInt()
    joint_areas = WoodNestedVectorDouble()
    joint_types = WoodVectorInt()

    wood_pybind11.joints(
        # input
        list_of_polylines_coordinates,
        0,
        # output
        element_pairs,
        joint_areas,
        joint_types,
    )

    print(
        "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________"
    )

    ###################################################################################################
    # conversion of output data and output
    ###################################################################################################
    return (
        WoodNestedVectorInt_to_lists(element_pairs),
        WoodNestedVectorDouble_to_polylines(joint_areas),
        WoodVectorInt_to_list(joint_types),
    )


def rtree(list_of_polylines):
    """Get neighbors of elements using a R-tree.

    Parameters
    ----------
    list_of_polylines : list[Polyline]
        list of polylines that represents the timber elements

    Returns
    -------
    tuple:
        list[list[int]] elements_neighbours,
        list[Mesh] elements_AABB
        list[Mesh] elements_OOBB
    """

    ###################################################################################################
    # conversion of input data
    ###################################################################################################
    list_of_polylines_coordinates = polylines_to_WoodNestedVectorDouble(
        list_of_polylines
    )

    ###################################################################################################
    # run the WOOD CPP code
    ###################################################################################################

    print(
        "\n___________________________________start_of_WOOD_____________________________________"
    )
    start_time = time.time()
    elements_neighbours = WoodNestedVectorInt()  # each vector consists of 2 indices
    elements_AABB = WoodNestedVectorDouble()  # each vector consists of 24 coordinates
    elements_OOBB = WoodNestedVectorDouble()  # each vector consists of 24 coordinates

    wood_pybind11.rtree(
        # input
        list_of_polylines_coordinates,
        # output
        elements_neighbours,
        elements_AABB,
        elements_OOBB,
    )

    print(
        "\n______________________________________ %s ms ______________________________________"
        % round((time.time() - start_time) * 1000.0, 2)
    )
    print(
        "\n____________________________________end_of_WOOD______________________________________"
    )

    ###################################################################################################
    # conversion of output data
    ###################################################################################################
    return (
        WoodNestedVectorInt_to_lists(elements_neighbours),
        WoodNestedVectorDouble_to_boxeslist(elements_AABB),
        WoodNestedVectorDouble_to_boxeslist(elements_OOBB),
    )
