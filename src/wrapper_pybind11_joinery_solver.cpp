#include "wrapper_pybind11_joinery_solver.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Converter methods, where np.asarray are mapped to Eigner matrix
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region converter


std::vector<RowMatrixXd>  result_from_polylines(CGAL_Polylines polylines) {

    std::vector< RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {

        const CGAL_Polyline& poly = *i;
        int n = poly.size();
        RowMatrixXd points(n, 3);

        for (int j = 0; j < n; j++) {
            points(j, 0) = (double)poly[j].x();
            points(j, 1) = (double)poly[j].y();
            points(j, 2) = (double)poly[j].z();
        }

        pointsets.push_back(points);
    }

    return pointsets;
}

std::vector<RowMatrixXd>  result_from_vector(IK::Vector_3* v) {
    std::vector< RowMatrixXd> pointsets;

    //for (auto i = 0; i < AABBs.size(); i++) {


    int n = 2;
    RowMatrixXd points(4, 3);

    points(0, 0) = (double)v[0].x();
    points(0, 1) = (double)v[0].y();
    points(0, 2) = (double)v[0].z();
    points(1, 0) = (double)v[1].x();
    points(1, 1) = (double)v[1].y();
    points(1, 2) = (double)v[1].z();
    points(2, 0) = (double)v[2].x();
    points(2, 1) = (double)v[2].y();
    points(2, 2) = (double)v[2].z();
    points(3, 0) = (double)v[3].x();
    points(3, 1) = (double)v[3].y();
    points(3, 2) = (double)v[3].z();

    pointsets.push_back(points);
    // }
    return pointsets;
}

std::vector<RowMatrixXd>  result_from_bbox(std::vector<CGAL::Bbox_3> AABBs) {
    std::vector< RowMatrixXd> pointsets;

    for (auto i = 0; i < AABBs.size(); i++) {


        int n = 2;
        RowMatrixXd points(n, 3);

        points(0, 0) = (double)AABBs[i].xmin();
        points(0, 1) = (double)AABBs[i].ymin();
        points(0, 2) = (double)AABBs[i].zmin();
        points(1, 0) = (double)AABBs[i].xmax();
        points(1, 1) = (double)AABBs[i].ymax();
        points(1, 2) = (double)AABBs[i].zmax();

        pointsets.push_back(points);
    }
    return pointsets;
}

std::vector<CGAL_Polyline>  polylines_from_vertices_and_faces(const  RowMatrixXd& V, const  RowMatrixXi& F) {
    //////////////////////////////////////////////////////////////////////////////
    //Convert Raw data to list of Polyline
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> polylinePairs(F.size());
    CGAL_Polyline pline;

    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {

        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            polylinePairs[counter] = pline;
            pline.clear();//Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }

    }
    return polylinePairs;
}

void  polylines_from_vertices_and_faces_and_properties(
    const  RowMatrixXd& polylines_vertices_XYZ,
    const  RowMatrixXi& polylines_vertices_count_int,
    const  RowMatrixXd& face_vectors_XYZ,
    const  RowMatrixXi& face_joints_types_int,
    const  RowMatrixXi& three_valence_element_indices_and_instruction,
    const  RowMatrixXd& default_parameters_for_joint_types_matrix,

    std::vector<CGAL_Polyline>& out_polyline_pairs,
    std::vector<std::vector<IK::Vector_3>>& out_insertion_vectors,
    std::vector<std::vector<int>>& out_joint_types,
    std::vector<std::vector<int>>& out_three_valence_element_indices_and_instruction,
    std::vector<double>& out_default_parameters_for_joint_types


) {

    //////////////////////////////////////////////////////////////////////////////
    //Convert Raw data to list of Polyline
    //////////////////////////////////////////////////////////////////////////////
    //std::vector<CGAL_Polyline> out_polyline_pairs; 
    out_polyline_pairs.reserve(polylines_vertices_count_int.size());

    //std::vector< std::vector<IK::Vector_3>> insertion_vectors;
    out_insertion_vectors.reserve(polylines_vertices_count_int.size());

    // std::vector< std::vector<int>> joint_types;
    out_joint_types.reserve(polylines_vertices_count_int.size());





    CGAL_Polyline pline;
    int counter = 0;
    int lastCount = polylines_vertices_count_int(counter, 0);
    for (int i = 0; i < polylines_vertices_XYZ.size() / 3; i++) {

        CGAL::Epick::Point_3 p(polylines_vertices_XYZ(i, 0), polylines_vertices_XYZ(i, 1), polylines_vertices_XYZ(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            out_polyline_pairs.push_back(pline);
            pline.clear();//Clear points from the polyline
            lastCount = polylines_vertices_count_int(++counter, 0); //Take next polyline Count
        }
    }

    if (face_vectors_XYZ.size() > 0) {
        //printf("999");
        std::vector<IK::Vector_3> vectors;
        counter = 0;
        int lastCount = polylines_vertices_count_int(counter, 0) + 1;
        for (int i = 0; i < face_vectors_XYZ.size() / 3; i++) {

            CGAL::Epick::Vector_3 v(face_vectors_XYZ(i, 0), face_vectors_XYZ(i, 1), face_vectors_XYZ(i, 2));
            vectors.push_back(v);

            if (vectors.size() == (lastCount)) {
                out_insertion_vectors.push_back(vectors);
                vectors.clear();//Clear points from the polyline
                lastCount = polylines_vertices_count_int(++counter, 0) + 1; //Take next polyline Count
            }

        }

    }

    if (face_joints_types_int.size() > 0) {
        //printf("888");
        std::vector<int> types;
        counter = 0;
        int lastCount = polylines_vertices_count_int(counter, 0) + 1;
        for (int i = 0; i < face_joints_types_int.size(); i++) {

            int id(face_joints_types_int(i, 0));
            types.push_back(id);

            if (types.size() == (lastCount)) {
                out_joint_types.push_back(types);
                types.clear();//Clear points from the polyline
                lastCount = polylines_vertices_count_int(++counter, 0) + 1; //Take next polyline Count
            }

        }

    }

    if (three_valence_element_indices_and_instruction.size() > 0) {
        out_three_valence_element_indices_and_instruction.reserve(three_valence_element_indices_and_instruction.size());



        for (int i = 0; i < three_valence_element_indices_and_instruction.size(); i += 4) {

            std::vector<int> ids
            {
            three_valence_element_indices_and_instruction(i + 0, 0),
            three_valence_element_indices_and_instruction(i + 1, 0),
            three_valence_element_indices_and_instruction(i + 2, 0),
            three_valence_element_indices_and_instruction(i + 3, 0),

            };

            out_three_valence_element_indices_and_instruction.push_back(ids);

        }
    }

    if (default_parameters_for_joint_types_matrix.size() > 0) {
        out_default_parameters_for_joint_types.reserve(default_parameters_for_joint_types_matrix.size());


        for (int i = 0; i < default_parameters_for_joint_types_matrix.size(); i++) {
            out_default_parameters_for_joint_types.push_back(default_parameters_for_joint_types_matrix(i, 0));

        }

    }






}


std::vector< RowMatrixXd>  result_from_polylinesVectorVector(std::vector < std::vector < CGAL_Polyline>> polylines) {

    std::vector< RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {

        for (auto j = i->begin(); j != i->end(); j++) {

            const CGAL_Polyline& poly = *j;
            int n = poly.size();
            RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++) {
                points(k, 0) = (double)poly[k].x();
                points(k, 1) = (double)poly[k].y();
                points(k, 2) = (double)poly[k].z();
            }

            pointsets.push_back(points);
        }
    }

    //printf("CPP number of points: %i ", pointsets.size());

    return pointsets;
}

std::vector< RowMatrixXd>  result_from_polylinesVector(std::vector < CGAL_Polyline>& polylines, bool debug) {

    std::vector< RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {



        const CGAL_Polyline& poly = *i;
        int n = poly.size();
        RowMatrixXd points(n, 3);

        for (int k = 0; k < n; k++) {
            points(k, 0) = (double)poly[k].hx();
            points(k, 1) = (double)poly[k].hy();
            points(k, 2) = (double)poly[k].hz();


        }

        pointsets.push_back(points);

    }

    //  printf("CPP number of points: %i ", pointsets.size());
    /*
    if (debug)
    {
        std::ofstream myfile;
        myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");

        for (auto i = polylines.begin(); i != polylines.end(); i++)
        {

            const CGAL_Polyline &poly = *i;
            int n = poly.size();
             RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++)
            {
                points(k, 0) = (double)poly[k].hx();
                points(k, 1) = (double)poly[k].hy();
                points(k, 2) = (double)poly[k].hz();
            }

            for (int k = 0; k < n; k++)
                myfile << poly[k].hx() << " " << poly[k].hy() << " " << poly[k].hz() << " "
                       << "\n";
            myfile << ("\n");
        }
        myfile.close();
    }
    */
    return pointsets;
}

std::tuple< std::vector< RowMatrixXd>, std::vector<int>  >  result_tuple_from_polylinesVector(std::vector < std::vector < CGAL_Polyline>>& polylines_vector, bool debug) {

    std::vector< RowMatrixXd> pointsets;
    std::vector<int> pointsets_groups_ids;


    int i = 0;
    for (auto& polylines : polylines_vector) {


        for (auto j = polylines.begin(); j != polylines.end(); j++) {



            const CGAL_Polyline& poly = *j;
            int n = poly.size();
            RowMatrixXd points(n, 3);


            for (int k = 0; k < n; k++) {
                points(k, 0) = (double)poly[k].hx();
                points(k, 1) = (double)poly[k].hy();
                points(k, 2) = (double)poly[k].hz();


            }

            pointsets.push_back(points);
            pointsets_groups_ids.push_back(i);

        }

        i++;
    }
    //  printf("CPP number of points: %i ", pointsets.size());
    /*
    if (debug) {
        std::ofstream myfile;
        myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");
        for (auto& polylines : polylines_vector) {

            for (auto i = polylines.begin(); i != polylines.end(); i++) {

                const CGAL_Polyline& poly = *i;
                int n = poly.size();
                 RowMatrixXd points(n, 3);

                for (int k = 0; k < n; k++) {
                    points(k, 0) = (double)poly[k].hx();
                    points(k, 1) = (double)poly[k].hy();
                    points(k, 2) = (double)poly[k].hz();
                }


                for (int k = 0; k < n; k++)
                    myfile << poly[k].hx() << " " << poly[k].hy() << " " << poly[k].hz() << " " << "\n";
                myfile << ("\n");
            }

        }
        myfile.close();
    }
    */
    return std::make_tuple(pointsets, pointsets_groups_ids);
}

#pragma endregion


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wrapped methods with inputs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pybind11_test() {
    printf("Hello from CPP Wood \n");
}


std::tuple< std::vector<RowMatrixXd>, std::vector<int>>  pybind11_get_connection_zones(
    Eigen::Ref<const RowMatrixXd>& polylines_vertices_XYZ,
    Eigen::Ref<const RowMatrixXi>& polylines_vertices_count_int,
    Eigen::Ref<const RowMatrixXd>& face_vectors_XYZ,
    Eigen::Ref<const RowMatrixXi>& face_joints_types_int,
    Eigen::Ref<const RowMatrixXi>& three_valence_element_indices_and_instruction,
    Eigen::Ref<const RowMatrixXd>& default_parameters_for_joint_types_matrix,

    int search_type = 1,
    double division_distance = 300,
    double shift = 0.6,
    int output_type = 4,
    int triangulate = 0

) {


    //////////////////////////////////////////////////////////////////////////////
    //Convert Raw data to list of Polyline
    //////////////////////////////////////////////////////////////////////////////
    //std::vector<element> elements;
    //std::vector<CGAL_Polyline> polyline_pairs =  polylines_from_vertices_and_faces(polylines_vertices_XYZ, polylines_vertices_count_int);
    std::vector<CGAL_Polyline> out_polyline_pairs;
    std::vector<std::vector<IK::Vector_3>> out_insertion_vectors;
    std::vector<std::vector<int>> out_joint_types;
    std::vector<std::vector<int>> out_three_valence_element_indices_and_instruction;
    std::vector<double> out_default_parameters_for_joint_types;


     polylines_from_vertices_and_faces_and_properties(

        polylines_vertices_XYZ,
        polylines_vertices_count_int,
        face_vectors_XYZ,
        face_joints_types_int,
        three_valence_element_indices_and_instruction,
        default_parameters_for_joint_types_matrix,

        out_polyline_pairs,
        out_insertion_vectors,
        out_joint_types,
        out_three_valence_element_indices_and_instruction,
        out_default_parameters_for_joint_types
    );

    std::vector<std::vector<CGAL_Polyline>> output;
    std::vector<std::vector<int>> top_face_triangulation;
    get_connection_zones(
        out_polyline_pairs,
        out_insertion_vectors,
        out_joint_types,
        out_three_valence_element_indices_and_instruction,

        output,
        top_face_triangulation,

        out_default_parameters_for_joint_types,
        search_type,
        division_distance,
        shift,
        output_type,
        triangulate

    );

    //////////////////////////////////////////////////////////////////////////////
    //Convert Output to Raw Data
    //////////////////////////////////////////////////////////////////////////////

    return  result_tuple_from_polylinesVector(output, true);




}


std::tuple< RowMatrixXd, RowMatrixXi> pybind11_closed_mesh_from_polylines(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F) {
    std::vector<CGAL_Polyline> polylines =  polylines_from_vertices_and_faces(V, F);
    return cgal_mesh_util::closed_mesh_from_polylines(polylines);
}

std::tuple< std::vector<RowMatrixXi>, RowMatrixXd, RowMatrixXd > pybind11_rtree(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F)//neihgbours per element, aabb, oobb
{
    //Do you need to delete the vector?
    //[0] [5, 6, 7]
    //[1] [4]
    // 
    //////////////////////////////////////////////////////////////////////////////
    //Convert raw-data to list of Polylines
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    input_polyline_pairs.reserve(F.size());

    CGAL_Polyline pline;
    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {

        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            input_polyline_pairs.push_back(pline);
            pline.clear();//Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }


    const int n = input_polyline_pairs.size() * 0.5;

    //////////////////////////////////////////////////////////////////////////////
    //Create elements, AABB, OBB, P, Pls, thickness
    //////////////////////////////////////////////////////////////////////////////
    std::vector<element> elements;
    std::vector< std::vector<IK::Vector_3>> input_insertion_vectors;
    std::vector< std::vector<int>> input_joint_types;
    get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);


    //////////////////////////////////////////////////////////////////////////////
    //Create joints, Perform Joint Area Search
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    // Create RTree
    //////////////////////////////////////////////////////////////////////////////

    RTree<int, double, 3> tree;

    //////////////////////////////////////////////////////////////////////////////
    // Insert AABB
    //////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < elements.size(); i++) {
        double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
        double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
        tree.Insert(min, max, i);
    }

    //////////////////////////////////////////////////////////////////////////////
    // Search Closest Boxes | Skip duplicates pairs | Perform callback with OBB
    //////////////////////////////////////////////////////////////////////////////
    std::vector<RowMatrixXi> elements_neigbhours;
    elements_neigbhours.reserve(elements.size());

    for (int i = 0; i < elements.size(); i++) {
        //CGAL_Debug("_____");

        std::vector<int> element_neigbhours;

        //std::vector<int> result;
        auto callback = [&element_neigbhours, i, &elements](int foundValue) -> bool
        {
            if (cgal_box_util::GetCollision(elements[i].oob, elements[foundValue].oob)) {
            //if (i < foundValue && cgal_box_util::GetCollision(elements[i].oob, elements[foundValue].oob)) {
                //element_neigbhours.push_back(i);
                element_neigbhours.push_back(foundValue);
            }
            return true;
        };

        double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
        double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
        int nhits = tree.Search(min, max, callback);//callback in this method call callback above

        //elements_neigbhours.push_back(element_neigbhours);

        //Convert vector to matrix
        RowMatrixXi element_neigbhours_matrix(1, element_neigbhours.size());

        for (int j = 0; j < element_neigbhours.size(); j ++) {
            //CGAL_Debug(element_neigbhours[j]);
            element_neigbhours_matrix(0, j) = element_neigbhours[j];
        }
     
        elements_neigbhours.push_back(element_neigbhours_matrix);
    }




    ////////////////////////////////////////////////////////////////////////////////
    //// Output AABB
    ////////////////////////////////////////////////////////////////////////////////
    RowMatrixXd elements_AABB(elements.size(),24);

    for (int i = 0; i < elements.size(); i++) {


        elements_AABB(i, 0) = elements[i].aabb.xmin();
        elements_AABB(i, 1) = elements[i].aabb.ymin();
        elements_AABB(i, 2) = elements[i].aabb.zmin();

        elements_AABB(i, 3) = elements[i].aabb.xmin();
        elements_AABB(i, 4) = elements[i].aabb.ymin();
        elements_AABB(i, 5) = elements[i].aabb.zmax();

        elements_AABB(i, 6) = elements[i].aabb.xmin();
        elements_AABB(i, 7) = elements[i].aabb.ymax();
        elements_AABB(i, 8) = elements[i].aabb.zmax();

        elements_AABB(i, 9) = elements[i].aabb.xmin();
        elements_AABB(i, 10) = elements[i].aabb.ymax();
        elements_AABB(i, 11) = elements[i].aabb.zmin();


        elements_AABB(i, 12) = elements[i].aabb.xmax();
        elements_AABB(i, 13) = elements[i].aabb.ymin();
        elements_AABB(i, 14) = elements[i].aabb.zmin();

        elements_AABB(i, 15) = elements[i].aabb.xmax();
        elements_AABB(i, 16) = elements[i].aabb.ymin();
        elements_AABB(i, 17) = elements[i].aabb.zmax();

        elements_AABB(i, 18) = elements[i].aabb.xmax();
        elements_AABB(i, 19) = elements[i].aabb.ymax();
        elements_AABB(i, 20) = elements[i].aabb.zmax();

        elements_AABB(i, 21) = elements[i].aabb.xmax();
        elements_AABB(i, 22) = elements[i].aabb.ymax();
        elements_AABB(i, 23) = elements[i].aabb.zmin();



    }

    ////////////////////////////////////////////////////////////////////////////////
    //// Output OOBB
    ////////////////////////////////////////////////////////////////////////////////

    RowMatrixXd elements_OOBB(elements.size(), 24);

    for (int i = 0; i < elements.size(); i++) {

        CGAL_Polyline corners;
        cgal_box_util::GetCorners(elements[i].oob, corners);

      for (int j = 0; j < 8; j++) {
          //int a = j * 3 + 0;
          //int b = j * 3 + 1;
          //int c = j * 3 + 2;
          //CGAL_Debug(j * 3 + 0);
          //CGAL_Debug(j * 3 + 1);
          //CGAL_Debug(j * 3 + 2);
          //CGAL_Debug(corners.size());
            elements_OOBB(i, j * 3 + 0) = corners[j].hx();
            elements_OOBB(i, j * 3 + 1) = corners[j].hy();
            elements_OOBB(i, j * 3 + 2) = corners[j].hz();
        }

    }

    //////////////////////////////////////////////////////////////////////////////
    // Output Tuple
    //////////////////////////////////////////////////////////////////////////////
    return std::make_tuple(elements_neigbhours, elements_AABB, elements_OOBB);
}

//element_pairs, joint areas and joint types
std::tuple< RowMatrixXi, std::vector<RowMatrixXd>, RowMatrixXi > pybind11_joints(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F, int& search_type)
{

//////////////////////////////////////////////////////////////////////////////
//Convert raw-data to list of Polylines
//////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    input_polyline_pairs.reserve(F.size());

    CGAL_Polyline pline;
    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {

        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            input_polyline_pairs.push_back(pline);
            pline.clear();//Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }


    const int n = input_polyline_pairs.size() * 0.5;

    //////////////////////////////////////////////////////////////////////////////
    //Create elements, AABB, OBB, P, Pls, thickness
    //////////////////////////////////////////////////////////////////////////////
    std::vector<element> elements;
    std::vector< std::vector<IK::Vector_3>> input_insertion_vectors;
    std::vector< std::vector<int>> input_joint_types;
    get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);


    //////////////////////////////////////////////////////////////////////////////
    //Create joints, Perform Joint Area Search
    //////////////////////////////////////////////////////////////////////////////

    auto joints = std::vector<joint>();
    auto joints_map = std::unordered_map<uint64_t, int>();
    rtree_search(elements, search_type, joints, joints_map);

    //////////////////////////////////////////////////////////////////////////////
    //Get element pairs, joint areas, join types
    //////////////////////////////////////////////////////////////////////////////
    RowMatrixXi element_pairs(joints.size(), 2);
    std::vector<RowMatrixXd> joint_areas;
    joint_areas.reserve(joints.size());
    RowMatrixXi joint_types(joints.size(),1);


   


    for (int i = 0; i < joints.size(); i++) {

        //element pairs
        element_pairs(i, 0) = joints[i].v0;
        element_pairs(i, 1) = joints[i].v1;


        //CGAL_Debug(" ");
        //for (auto& pp : joints[i].joint_area)
        //    CGAL_Debug(pp.hx(), pp.hy(), pp.hz());

        //joint areas
        RowMatrixXd current_joint_areas(joints[i].joint_area.size(),3);

        for (size_t j = 0; j < joints[i].joint_area.size(); j++) {
            current_joint_areas(j, 0) = joints[i].joint_area[j].hx();
            current_joint_areas(j, 1) = joints[i].joint_area[j].hy();
            current_joint_areas(j, 2) = joints[i].joint_area[j].hz();
        }
        joint_areas.emplace_back(current_joint_areas);

        //joint types
        joint_types(i, 0) = joints[i].type;
       


    }






    //////////////////////////////////////////////////////////////////////////////
    // Output Tuple
    //////////////////////////////////////////////////////////////////////////////
    return std::make_tuple(element_pairs, joint_areas, joint_types);
}
