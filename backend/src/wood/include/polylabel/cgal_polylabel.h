#pragma once

#include <polylabel/polylabel.h>
#include <CGAL/Largest_empty_iso_rectangle_2.h>
namespace cgal_polylabel
{

    namespace internal
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in clipper_util to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline double length(double x, double y, double z)
        {

            double len;
            x = fabs(x);
            y = fabs(y);
            z = fabs(z);
            if (y >= x && y >= z)
            {
                len = x;
                x = y;
                y = len;
            }
            else if (z >= x && z >= y)
            {
                len = x;
                x = z;
                z = len;
            }

            // 15 September 2003 Dale Lear
            //     For small denormalized doubles (positive but smaller
            //     than DBL_MIN), some compilers/FPUs set 1.0/x to +INF.
            //     Without the ON_DBL_MIN test we end up with
            //     microscopic vectors that have infinte length!
            //
            //     This code is absolutely necessary.  It is a critical
            //     part of the bug fix for RR 11217.
            if (x > ON_DBL_MIN)
            {
                y /= x;
                z /= x;
                len = x * sqrt(1.0 + y * y + z * z);
            }
            else if (x > 0.0 && ON_IS_FINITE(x))
                len = x;
            else
                len = 0.0;

            return len;
        }

        inline bool unitize(IK::Vector_3 &vector)
        {
            bool rc = false;
            // Since x,y,z are floats, d will not be denormalized and the
            // ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

            double d = length(vector.hx(), vector.hy(), vector.hz());
            if (d > 0.0)
            {
                double dx = vector.hx();
                double dy = vector.hy();
                double dz = vector.hz();
                vector = IK::Vector_3(
                    (dx / d),
                    (dy / d),
                    (dz / d));
                rc = true;
            }
            return rc;
        }

        inline CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, IK::Vector_3 &x0, IK::Vector_3 &y0, IK::Vector_3 &z0)
        {
            unitize(x0);
            unitize(y0);
            unitize(z0);

            // Move to origin -> T0 translates point P0 to (0,0,0)
            CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

            // Rotate ->
            CGAL::Aff_transformation_3<IK> f0(
                x0.x(), x0.y(), x0.z(),
                y0.x(), y0.y(), y0.z(),
                z0.x(), z0.y(), z0.z());

            return f0 * t0;
        }

        inline CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane)
        {
            auto x0 = plane.base1();
            auto y0 = plane.base2();
            auto z0 = plane.orthogonal_vector();
            cgal_vector_util::Unitize(x0);
            cgal_vector_util::Unitize(y0);
            cgal_vector_util::Unitize(z0);

            // Move to origin -> T0 translates point P0 to (0,0,0)
            CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

            // Rotate ->
            CGAL::Aff_transformation_3<IK> f0(
                x0.x(), x0.y(), x0.z(),
                y0.x(), y0.y(), y0.z(),
                z0.x(), z0.y(), z0.z());

            return f0 * t0;
        }

        inline void get_average_normal(const CGAL_Polyline &polyline, IK::Vector_3 &average_normal)
        {
            size_t len = CGAL::squared_distance(polyline.front(), polyline.back()) < wood_globals::DISTANCE_SQUARED ? polyline.size() - 1 : polyline.size();
            average_normal = IK::Vector_3(0, 0, 0);

            for (int i = 0; i < len; i++)
            {
                auto prev = ((i - 1) + len) % len;
                auto next = ((i + 1) + len) % len;
                average_normal = average_normal + CGAL::cross_product(polyline[i] - polyline[prev], polyline[next] - polyline[i]);
            }
        }

        inline void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &center, IK::Plane_3 &plane)
        {
            // origin
            center = polyline[0];

            // plane
            IK::Vector_3 average_normal;
            get_average_normal(polyline, average_normal);
            plane = IK::Plane_3(center, average_normal);
        }

        inline void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Vector_3 &x_axis, IK::Vector_3 &y_axis, IK::Vector_3 &z_axis)
        {
            // origin
            origin = polyline[0];

            // plane
            IK::Vector_3 average_normal;
            get_average_normal(polyline, average_normal);
            IK::Plane_3 plane(origin, average_normal);

            x_axis = plane.base1();
            y_axis = plane.base2();
            z_axis = plane.orthogonal_vector();
        }

        inline void get_average_plane_axes_oriented_to_longest_edge(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Vector_3 &x_axis, IK::Vector_3 &y_axis, IK::Vector_3 &z_axis)
        {
            size_t len = CGAL::squared_distance(polyline.front(), polyline.back()) < wood_globals::DISTANCE_SQUARED ? polyline.size() - 1 : polyline.size();
            z_axis = IK::Vector_3(0, 0, 0);

            double max_length = 0;
            for (int i = 0; i < len; i++)
            {
                auto prev = ((i - 1) + len) % len;
                auto next = ((i + 1) + len) % len;
                z_axis = z_axis + CGAL::cross_product(polyline[i] - polyline[prev], polyline[next] - polyline[i]);

                double temp_max_length = CGAL::squared_distance(polyline[i], polyline[next]);
                if (temp_max_length > max_length)
                {
                    max_length = temp_max_length;
                    x_axis = polyline[next] - polyline[i];
                    origin = polyline[i];
                }
            }
            y_axis = CGAL::cross_product(x_axis, z_axis);
        }

        /**
         * create point in a circle
         * the first four inputs are needed to orient the points in 3d
         * WARNING user must not use this method - this method is used by get_polylabel_circle_division_points method
         * @param [in] origin plane origin
         * @param [in] x_axis plane x axis
         * @param [in] y_axis plane y axis
         * @param [in] z_axis plane z axis
         * @param [out] points point divisions
         * @param [in] number_of_chunks circle divisions
         * @param [in] radius circle radius
         */
        inline void circle_points(const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis, std::vector<IK::Point_3> &points, int number_of_chunks = 4, double radius = 150)
        {

            double pi_to_radians = 3.14159265358979323846 / 180.0;
            double degrees = 0; // <-- correction
            double chunk_angle = 360 / number_of_chunks;
            points.reserve(number_of_chunks);
            CGAL::Aff_transformation_3<IK> xy_to_plane = cgal_xform_util::XYToPlane(origin, x_axis, y_axis, z_axis);

            for (int i = 0; i < number_of_chunks; i++)
            {
                degrees = i * chunk_angle;                       // <-- correction
                float radian = ((45 + degrees) * pi_to_radians); // <-- correction
                IK::Point_3 p(radius * cos(radian), radius * sin(radian), 0);
                points.emplace_back(p.transform(xy_to_plane));
            }
        }

        /**
         * get transformation from 3D to 2D and invcerse from a polyline
         * the 3D plane is computed from the start point of the polyline
         * and an the sum of the consecutive edges cross products
         *
         * @param [in] polyline input CGAL polyline
         * @param [out] xform_to_xy transformation from 3D to XY
         * @param [out] xform_to_xy_inv transformation from XY to 3D
         * @return returns true if the polyline has more than 3 points, else not valid
         */
        inline bool orient_polyline_to_xy_and_back(CGAL_Polyline &polyline, CGAL::Aff_transformation_3<IK> &xform_to_xy, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
        {

            /////////////////////////////////////////////////////////////////////////////////////
            // Orient from 3D to 2D
            /////////////////////////////////////////////////////////////////////////////////////
            if (polyline.size() < 3)
                return false;

            /////////////////////////////////////////////////////////////////////////////////////
            // Get center polyline plane
            /////////////////////////////////////////////////////////////////////////////////////
            IK::Point_3 center;
            IK::Plane_3 plane;
            cgal_polyline_util::get_fast_plane(polyline, center, plane);

            /////////////////////////////////////////////////////////////////////////////////////
            // Create Transformation and assign outputs
            /////////////////////////////////////////////////////////////////////////////////////
            xform_to_xy = cgal_xform_util::PlaneToXY(polyline[0], plane);
            xform_to_xy_inv = xform_to_xy.inverse();
            return true;
        }
    }

    /**
     * inscribe circle in a polygon (with or without holes)
     * the polyline is oriented to XY plane using the input plane
     * then oriented back to 3D
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     * EXAMPLE:
     *   std::vector<CGAL_Polyline> polylines;
     *
     *    CGAL_Polyline p0{
     *        IK::Point_3(-51.268164698738, 124.403259575835, 0),
     *        IK::Point_3(-49.9053069162896, 125.207270883064, 0),
     *        IK::Point_3(-49.2387999055025, 123.598641360433, 0),
     *        IK::Point_3(-51.268164698738, 124.403259575835, 0),
     *    };
     *
     *    CGAL_Polyline p1{
     *        IK::Point_3(-80.6608706869908, 113.550451407233, 0),
     *        IK::Point_3(-105.295437496349, 124.537127782527, 0),
     *        IK::Point_3(-76.4480884206953, 152.30682138802, 0),
     *        IK::Point_3(-23.6934094178782, 139.390066049447, 0),
     *        IK::Point_3(-40.1282334662578, 100.282298806669, 0),
     *        IK::Point_3(-69.6350088663798, 87.7972305312764, 0),
     *        IK::Point_3(-74.8538706538141, 108.464053813429, 0),
     *        IK::Point_3(-80.6608706869908, 113.550451407233, 0),
     *    };
     *
     *    CGAL_Polyline p2{
     *        IK::Point_3(-73.4808769486925, 119.997690099402, 0),
     *        IK::Point_3(-90.6424155869293, 123.188472670033, 0),
     *        IK::Point_3(-77.6978215210285, 136.742511096638, 0),
     *        IK::Point_3(-73.4808769486925, 119.997690099402, 0),
     *    };
     *
     *    polylines.push_back(p0);
     *    polylines.push_back(p1);
     *    polylines.push_back(p2);
     *
     *    std::tuple<IK::Point_3, IK::Plane_3, double> result = get_polylabel(polylines, 1.0);
     *    std::cout << std::get<0>(result).hx() << " " << std::get<0>(result).hy() << " " << std::get<0>(result).hz() << std::endl;
     *    std::cout << std::get<1>(result).orthogonal_vector().hx() << " " << std::get<1>(result).orthogonal_vector().hy() << " " << std::get<1>(result).orthogonal_vector().hz() << std::endl;
     *    std::cout << std::get<2>(result) << std::endl;
     *
     * RESULT:
     * Center -58.9292 109.972 -0
     * ZAxis 0 0 -8.18464
     * Radius 15.8093
     *
     *
     * @param polylines planar polylines rotated in space
     * @param precision precision, the smaller the number the less precise the result, default 1.0
     * @return a tuple that represents the circle (center, plane, radius)
     */
    inline std::tuple<IK::Point_3, IK::Plane_3, double> get_polylabel(const std::vector<CGAL_Polyline> &polylines, double precision = 1.0)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // copy polylines for orienting from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> polylines_copy = polylines;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation to XY plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 origin;
        IK::Plane_3 plane;
        cgal_polyline_util::get_fast_plane(polylines_copy[0], origin, plane);

        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(origin, plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        for (auto &polyline : polylines_copy)
            cgal_polyline_util::Transform(polyline, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Sort polylines based on bounding-box to detect which polylines are holes, this only works if the polygon does not have holes within holes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<double> len = std::vector<double>(polylines_copy.size());
        std::vector<int> ids(len.size());
        std::iota(begin(ids), end(ids), 0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get bounding box length of polylines_copy
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (len.size() > 0)
        {
            for (int i = 0; i < polylines_copy.size(); i++)
            {
                CGAL::Bbox_3 AABB = CGAL::bbox_3(polylines_copy[i].begin(), polylines_copy[i].end(), IK());
                auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
                auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
                len[i] = CGAL::squared_distance(p0, p1);
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Sort polylines_copy indices by the bounding box length
            // WARNING orientation is not checked, which should be fine with the Polylabel algorithm
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            std::vector<int> ids(len.size());
            std::sort(begin(ids), end(ids), [&](int ia, int ib)
                      { return len[ia] < len[ib]; });
            std::reverse(begin(ids), end(ids));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert polylines_copy to Polylabel datastructure, be aware that the polyline considers all polygons closed, so skip the last point
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        mapbox::geometry::polygon<double> polygon_with_holes(polylines_copy.size());

        for (int i = 0; i < polylines_copy.size(); i++)
        {
            for (int j = 0; j < polylines_copy[ids[i]].size() - 1; j++)
            {
                polygon_with_holes[i].emplace_back(polylines_copy[ids[i]][j].x(), polylines_copy[ids[i]][j].y());
                // std::cout << polylines_copy[ids[i]][j].x() << " " << polylines_copy[ids[i]][j].y() << std::endl;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Run the Polylabel algorithm, WARNING numbers are in 2D, first two doubles are center, the third is the radius
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::array<double, 3> center_and_radius = mapbox::polylabel(polygon_with_holes, 1.0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert from the Polylabel datastructure to CGAL
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 center(center_and_radius[0], center_and_radius[1], 0);
        center = center.transform(xform_toXY_Inv);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return std::tuple<IK::Point_3, IK::Plane_3, double>(center, plane, center_and_radius[2]);
    }

    /**
     * get points inscribed in a polylable circle
     * @param [in] division_direction_in_3d division direction
     * @param [in] polylines list of polylines that representes the input polylines for the polylabel algorithm
     * @param [out] points division points
     * @param [in] division number of points
     * @param [in] precision tolerance for the polylabel algorithm
     */
    inline void get_polylabel_circle_division_points(const IK::Vector_3 &division_direction_in_3d, const std::vector<CGAL_Polyline> &polylines, std::vector<IK::Point_3> &points, int division = 4, double scale = 0.75, double precision = 1.0, bool orient_to_closest_edge = true)
    {
        // run the polylabel algorithm
        std::tuple<IK::Point_3, IK::Plane_3, double> circle = get_polylabel(polylines, precision);
        IK::Vector_3 center(std::get<0>(circle).hx(), std::get<0>(circle).hy(), std::get<0>(circle).hz());
        bool is_direction_valid = division_direction_in_3d.hx() != 0.0 && division_direction_in_3d.hy() != 0.0 && division_direction_in_3d.hz() != 0.0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Find closest edge direction to the center
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        IK::Point_3 center_point(std::get<0>(circle).hx(), std::get<0>(circle).hy(), std::get<0>(circle).hz());
        double distance_from_center_to_polyline_edge = DBL_MAX;
        size_t edge_id0 = 0;
        size_t edge_id1 = 0;
        if (orient_to_closest_edge)
        {

            for (auto i = 0; i < polylines.size(); i++)
            {
                for (auto j = 0; j < polylines[i].size() - 1; j++)
                {

                    IK::Segment_3 polyline_segment(polylines[i][j], polylines[i][j + 1]);
                    IK::Point_3 projection = polyline_segment.supporting_line().projection(center_point);

                    // closest point paraneters
                    bool has_on = false;
                    const IK::Vector_3 polyline_segment_vector = polyline_segment.to_vector();
                    const double polyline_segment_vector_squared_len = polyline_segment_vector.squared_length();
                    double t = 0;

                    if (polyline_segment_vector.squared_length() > 0.0)
                    {
                        if ((projection - polyline_segment[0]).squared_length() <= (projection - polyline_segment[1]).squared_length())
                            t = ((projection - polyline_segment[0]) * polyline_segment_vector) / polyline_segment_vector_squared_len;
                        else
                            t = 1.0 + ((projection - polyline_segment[1]) * polyline_segment_vector) / polyline_segment_vector_squared_len;

                        has_on = t >= 0 && t <= 1;
                    }
                    else
                    { // (GBA) Closest point to a degenerate line works as well
                        t = 0.0;
                        has_on = false;
                    }

                    // check if the point is on the edge
                    if (has_on)
                    {
                        double temp_distance_from_center_to_polyline_edge = CGAL::squared_distance(center_point, projection);
                        if (temp_distance_from_center_to_polyline_edge < distance_from_center_to_polyline_edge)
                        {
                            distance_from_center_to_polyline_edge = temp_distance_from_center_to_polyline_edge;
                            edge_id0 = i;
                            edge_id1 = j;
                        }
                    }
                }
            }
        }

        // orientation direction
        IK::Vector_3 division_direction_in_3d_ = orient_to_closest_edge ? polylines[edge_id0][edge_id1 + 1] - polylines[edge_id0][edge_id1 + 0] : IK::Vector_3(division_direction_in_3d.hx(), division_direction_in_3d.hy(), division_direction_in_3d.hz());

        // plane vectors for orienting again to 2d for circle points dvisions
        IK::Vector_3 x_axis = is_direction_valid || orient_to_closest_edge ? division_direction_in_3d_ : std::get<1>(circle).base1();
        IK::Vector_3 y_axis = is_direction_valid || orient_to_closest_edge ? CGAL::cross_product(division_direction_in_3d_, std::get<1>(circle).orthogonal_vector()) : std::get<1>(circle).base2();
        IK::Vector_3 z_axis = std::get<1>(circle).orthogonal_vector();

        // run the division method which is the output -> points
        internal::circle_points(center, x_axis, y_axis, z_axis, points, division, std::get<2>(circle) * scale);
    }

    /**
     * iscribe ractangle in a polygon and divide its edges into points
     * https://doc.cgal.org/latest/Inscribed_areas/index.html
     * Step 3 - Get center and a plane by the polylabel algorithm
     * Step 4 - Draw rectangle in the circle in the orientation of the bounding rectangle
     * Step 5 - Divide the rectangle into points
     * Step 6 - Orient and rotate to 3D
     *
     * @param [in] polygons input polylines (with holes)
     * @param [out] result output rectangle
     * @param [in] division_distance division distance of the edges
     * @return bool flag if the result is valid
     */

    inline bool inscribe_rectangle(const std::vector<CGAL_Polyline> &polylines, std::vector<CGAL_Polyline> &result, std::vector<IK::Point_3> &points, IK::Segment_3 &direction, double scale = 0, double precision = 1.0, double rectangle_division_distance = 10)
    {
        // issues
        // 1 - Check which line is good for oriented display ir by scaling a little git
        // 2 - exten rectangle by clipper intersection lines, and output the biggest one

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // copy polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> polylines_copy = polylines;
        // Clipper2Lib::Paths64 polylines_copy_clipper;

        // IK::Point_3 origin;
        // IK::Plane_3 plane;
        // cgal_polyline_util::get_fast_plane(polylines_copy[0], origin, plane);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // user direction
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        IK::Point_3 o;
        IK::Vector_3 x, y, z;
        if (direction.squared_length() > wood_globals::DISTANCE_SQUARED)
        {

            internal::get_fast_plane(polylines_copy[0], o, x, y, z);
            x = direction.to_vector();
            y = CGAL::cross_product(x, z);
        }
        else
        {
            // this was never checked
            internal::get_average_plane_axes_oriented_to_longest_edge(polylines_copy[0], o, x, y, z);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation to XY plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = internal::plane_to_xy(o, x, y, z);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Count how many points we have
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        size_t point_count = 0;

        for (auto &polyline : polylines_copy)
            point_count += polyline.size();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Compute 2D bounding box to get a division step, the division step is equal to = diagonal_length/(20*precision)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_2> points_2d;
        points_2d.reserve(point_count);

        for (auto &polyline : polylines_copy)
        {
            cgal_polyline_util::Transform(polyline, xform_toXY);
            for (auto &p : polyline)
                points_2d.push_back(IK::Point_2(p.hx(), p.hy()));
            points_2d.pop_back();
        }

        CGAL::Bbox_2 bbox = CGAL::bbox_2(points_2d.begin(), points_2d.end(), IK());
        auto diagonal = {IK::Point_3(bbox.xmin(), bbox.ymin(), 0), IK::Point_3(bbox.xmax(), bbox.ymax(), 0)};
        double squared_diagonal_length = CGAL::squared_distance(IK::Point_3(bbox.xmin(), bbox.ymin(), 0), IK::Point_3(bbox.xmax(), bbox.ymax(), 0));
        double step_division = std::sqrt(squared_diagonal_length) / (50.0 * precision);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get iso rectangle as the first input for the algorithm
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Iso_rectangle_2 bbox_rectangle(bbox);
        CGAL::Largest_empty_iso_rectangle_2<IK> empty_rect(bbox_rectangle); //(IK::Iso_rectangle_2(bbox));

        // points.emplace_back(IK::Point_3(bbox_rectangle.xmin(), bbox_rectangle.ymin(), 0));
        // points.emplace_back(IK::Point_3(bbox_rectangle.xmax(), bbox_rectangle.ymax(), 0));

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Interpolate edge points in by the precomputed division step
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline division_points;
        for (auto &polyline : polylines_copy)
            for (int i = 0; i < polyline.size() - 1; i++)
            {
                division_points.clear();
                int divisions = (int)std::min(100.0, std::sqrt(CGAL::squared_distance(polyline[i], polyline[i + 1])) / step_division);
                cgal_vector_util::interpolate_points(polyline[i], polyline[i + 1], divisions, division_points, 2);

                for (auto &point : division_points)
                    empty_rect.push_back(IK::Point_2(point.hx(), point.hy()));

                // points.insert(points.end(), division_points.begin(), division_points.end());
            }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get the largest empty iso rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Iso_rectangle_2 rectangle = empty_rect.get_largest_empty_iso_rectangle();
        double x_dist = (1 - scale) * std::abs(rectangle.max().hx() - rectangle.min().hx());
        double y_dist = (1 - scale) * std::abs(rectangle.max().hy() - rectangle.min().hy());

        CGAL_Polyline rect = {
            IK::Point_3(rectangle.min().hx() + x_dist, rectangle.min().hy() + y_dist, 0),
            IK::Point_3(rectangle.min().hx() + x_dist, rectangle.max().hy() - y_dist, 0),
            IK::Point_3(rectangle.max().hx() - x_dist, rectangle.max().hy() - y_dist, 0),
            IK::Point_3(rectangle.max().hx() - x_dist, rectangle.min().hy() + y_dist, 0),
            IK::Point_3(rectangle.min().hx() + x_dist, rectangle.min().hy() + y_dist, 0),
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient Back to 3D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (auto &point : rect)
            point = point.transform(xform_toXY_Inv);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < rect.size() - 1; i++)
        {
            division_points.clear();
            int divisions = (int)std::min(25.0, std::sqrt(CGAL::squared_distance(rect[i], rect[i + 1])) / rectangle_division_distance);
            cgal_vector_util::interpolate_points(rect[i], rect[i + 1], divisions, division_points, 2);
            points.insert(points.end(), division_points.begin(), division_points.end());
        }

        // points = rect;
        result = {
            // diagonal,
            polylines[0],
            rect};
        return true;
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Sort polylines based on bounding-box to detect which polylines are holes, this only works if the polygon does not have holes within holes
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // std::vector<double> len = std::vector<double>(polylines_copy.size());
        // std::vector<size_t> ids(len.size());
        // std::iota(begin(ids), end(ids), 0);

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Get bounding box length of polylines_copy
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // CGAL_Polyline diagonal;
        // for (auto i = 0; i < polylines_copy.size(); i++)
        // {
        //     CGAL::Bbox_3 AABB = CGAL::bbox_3(polylines_copy[i].begin(), polylines_copy[i].end(), IK());
        //     auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
        //     auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
        //     len[i] = std::sqrt(CGAL::squared_distance(p0, p1));
        // }

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Sort polylines_copy indices by the bounding box length
        // // WARNING orientation is not checked, which should be fine with the Polylabel algorithm
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // std::sort(begin(ids), end(ids), [&](size_t ia, size_t ib)
        //           { return len[ia] < len[ib]; });
        // std::reverse(begin(ids), end(ids));

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Convert polylines_copy to Polylabel datastructure, be aware that the polyline considers all polygons closed, so skip the last point
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // mapbox::geometry::polygon<double> polygon_with_holes(polylines_copy.size());

        // for (auto i = 0; i < polylines_copy.size(); i++)
        //     for (auto j = 0; j < polylines_copy[ids[i]].size() - 1; j++)
        //         polygon_with_holes[i].emplace_back(polylines_copy[ids[i]][j].x(), polylines_copy[ids[i]][j].y());

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Run the Polylabel algorithm, WARNING numbers are in 2D, first two doubles are center, the third is the radius
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // std::array<double, 3> center_and_radius = mapbox::polylabel(polygon_with_holes, precision);

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Find closest edge direction to the center
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // IK::Point_3 center(center_and_radius[0], center_and_radius[1], 0);
        // double distance_from_center_to_polyline_edge = DBL_MAX;
        // size_t edge_id0 = 0;
        // size_t edge_id1 = 0;
        // for (auto i = 0; i < polylines_copy.size(); i++)
        // {
        //     for (auto j = 0; j < polylines_copy[i].size() - 1; j++)
        //     {

        //         IK::Segment_3 polyline_segment(polylines_copy[i][j], polylines_copy[i][j + 1]);
        //         IK::Point_3 projection = polyline_segment.supporting_line().projection(center);

        //         // closest point paraneters
        //         bool has_on = false;
        //         const IK::Vector_3 polyline_segment_vector = polyline_segment.to_vector();
        //         const double polyline_segment_vector_squared_len = polyline_segment_vector.squared_length();
        //         double t = 0;

        //         if (polyline_segment_vector.squared_length() > 0.0)
        //         {
        //             if ((projection - polyline_segment[0]).squared_length() <= (projection - polyline_segment[1]).squared_length())
        //                 t = ((projection - polyline_segment[0]) * polyline_segment_vector) / polyline_segment_vector_squared_len;
        //             else
        //                 t = 1.0 + ((projection - polyline_segment[1]) * polyline_segment_vector) / polyline_segment_vector_squared_len;

        //             has_on = t >= 0 && t <= 1;
        //         }
        //         else
        //         { // (GBA) Closest point to a degenerate line works as well
        //             t = 0.0;
        //             has_on = false;
        //         }

        //         if (has_on)
        //         {
        //             double temp_distance_from_center_to_polyline_edge = CGAL::squared_distance(center, projection);
        //             if (temp_distance_from_center_to_polyline_edge < distance_from_center_to_polyline_edge)
        //             {
        //                 distance_from_center_to_polyline_edge = temp_distance_from_center_to_polyline_edge;
        //                 edge_id0 = i;
        //                 edge_id1 = j;
        //             }
        //         }
        //     }
        // }
        // std::cout << edge_id0 << " " << edge_id1 << "\n";

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Direction
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // IK::Vector_3 x_axis = polylines_copy[edge_id0][edge_id1 + 1] - polylines_copy[edge_id0][edge_id1];
        // double angle = 0.785398163397448309616; // 1.57079632679489661923;
        // // angle *= 0;
        // x_axis = IK::Vector_3(x_axis.hx() * std::cos(angle) - x_axis.hy() * std::sin(angle), x_axis.hx() * std::sin(angle) + x_axis.hy() * std::cos(angle), 0);
        // cgal_vector_util::Unitize(x_axis);

        // IK::Vector_3 z_axis(0, 0, 1);
        // IK::Vector_3 y_axis = CGAL::cross_product(x_axis, z_axis);

        // // run the division method which is the output -> points
        // std::vector<IK::Point_3> four_points;
        // internal::circle_points(IK::Vector_3(center_and_radius[0], center_and_radius[1], 0), x_axis, y_axis, z_axis, four_points, 4, std::get<2>(center_and_radius));
        // four_points.emplace_back(four_points.front());
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Draw two diagoanals, the length of the x,y axes are equal to the biggest bounding-rectangle diagonal length
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // IK::Vector_2 x_axis(len[ids[0]], 0);
        // // IK::Vector_2 y_axis(0, len[ids[0]]);

        // // double angle = 1.57079632679489661923;
        // // IK::Vector_3 y_axis(x_axis.hx() * std::cos(angle) - x_axis.hy() * std::sin(angle), x_axis.hx() * std::sin(angle) + x_axis.hy() * std::cos(angle), 0);
        // x_axis *= len[ids[0]];
        // y_axis *= len[ids[0]];
        // Clipper2Lib::Paths64 segment0{{Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] - x_axis.hx() - y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] - x_axis.hy() - y_axis.hy()))),
        //                                Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] + x_axis.hx() + y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] + x_axis.hy() + y_axis.hy())))}};

        // Clipper2Lib::Paths64 segment1{{Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] + x_axis.hx() - y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] + x_axis.hy() - y_axis.hy()))),
        //                                Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] - x_axis.hx() + y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] - x_axis.hy() + y_axis.hy())))}};

        // Clipper2Lib::Paths64 paths;
        // clipper_util::internal::cgalpolylines_to_clipper_2d(polylines_copy, paths);

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Intersect diagonals with all the polygons
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Clipper2Lib::Clipper64 c0;
        // c0.AddOpenSubject(segment0);
        // c0.AddClip(paths);
        // Clipper2Lib::Paths64 soln0, soln_open0;
        // c0.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::NonZero, soln0, soln_open0);

        // Clipper2Lib::Clipper64 c1;
        // c1.AddOpenSubject(segment1);
        // c1.AddClip(paths);
        // Clipper2Lib::Paths64 soln1, soln_open1;
        // c1.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::NonZero, soln1, soln_open1);

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Find clipper cut lines closest to the center and select them
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // if (soln_open0.size() == 0 || soln_open1.size() == 0)
        //     return false;

        // Clipper2Lib::Point64 center_clipper(
        //     (int64_t)(wood_globals::CLIPPER_SCALE * center_and_radius[0]),
        //     (int64_t)(wood_globals::CLIPPER_SCALE * center_and_radius[1]));

        // int64_t squared_distance_to_center = INT64_MAX;
        // size_t id0 = 0;
        // double min_radius;
        // for (size_t i = 0; i < soln_open0.size(); i++)
        // {
        //     int64_t temp_squared_distance_to_center = std::pow((center_clipper.x - soln_open0[i][0].x), 2) + std::pow((center_clipper.y - soln_open0[i][0].y), 2);
        //     if (temp_squared_distance_to_center < squared_distance_to_center)
        //     {
        //         // std::cout << squared_distance_to_center << " " << temp_squared_distance_to_center << " " << soln_open0.size() << "\n";
        //         squared_distance_to_center = temp_squared_distance_to_center;
        //         id0 = i;
        //     }
        // }

        // squared_distance_to_center = INT64_MAX;
        // size_t id1 = 0;
        // for (size_t i = 0; i < soln_open1.size(); i++)
        // {

        //     int64_t temp_squared_distance_to_center = std::pow((center_clipper.x - soln_open1[i][0].x), 2) + std::pow((center_clipper.y - soln_open1[i][0].y), 2);
        //     if (temp_squared_distance_to_center < squared_distance_to_center)
        //     {
        //         // std::cout << squared_distance_to_center << " " << temp_squared_distance_to_center << " " << soln_open1.size() << "\n";
        //         squared_distance_to_center = temp_squared_distance_to_center;
        //         id1 = i;
        //     }
        // }
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Find minimal radius
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // for (auto &point)
        // //  Clipper2Lib::Paths64 segment0_clipped = Clipper2Lib::Intersect(paths, segment0, Clipper2Lib::FillRule::NonZero);
        // //  Clipper2Lib::Paths64 segment1_clipped = Clipper2Lib::Intersect(paths, segment1, Clipper2Lib::FillRule::NonZero);

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // Step 6 - Convert to CGAL polyline and rotate to axis and orient to 3D
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // CGAL_Polyline segment0_clipped, segment1_clipped;
        // CGAL::Aff_transformation_3<IK> identity = CGAL::Identity_transformation();
        // clipper_util::internal::clipper_to_cgalpolyline_3d(soln_open0[id0], identity, segment0_clipped, false); // xform_toXY_Inv
        // clipper_util::internal::clipper_to_cgalpolyline_3d(soln_open1[id1], identity, segment1_clipped, false); // xform_toXY_Inv
        //                                                                                                         // clipper_util::internal::clipper_to_cgalpolyline_3d(segment0[0], identity, segment0_clipped, false); // xform_toXY_Inv
        //                                                                                                         // clipper_util::internal::clipper_to_cgalpolyline_3d(segment1[0], identity, segment1_clipped, false); // xform_toXY_Inv

        // CGAL_Polyline seg = {polylines_copy[edge_id0][edge_id1 + 1], polylines_copy[edge_id0][edge_id1]};
        // result = {
        //     seg, //
        //     polylines_copy[0],
        //     segment0_clipped,
        //     segment1_clipped,
        //     four_points};

        return true;
    }
    // inline bool inscribe_rectangle(const std::vector<CGAL_Polyline> &polylines, std::vector<CGAL_Polyline> &result, double division_distance = 0, double precision = 1.0)
    // {
    //     // issues
    //     // 1 - Check which line is good for oriented display ir by scaling a little git
    //     // 2 - exten rectangle by clipper intersection lines, and output the biggest one

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // copy polylines
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     std::vector<CGAL_Polyline> polylines_copy = polylines;
    //     Clipper2Lib::Paths64 polylines_copy_clipper;

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Create Transformation to XY plane
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     IK::Point_3 origin;
    //     IK::Plane_3 plane;
    //     cgal_polyline_util::get_fast_plane(polylines_copy[0], origin, plane);

    //     CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(origin, plane);
    //     CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
    //     for (auto &polyline : polylines_copy)
    //         cgal_polyline_util::Transform(polyline, xform_toXY);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Sort polylines based on bounding-box to detect which polylines are holes, this only works if the polygon does not have holes within holes
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     std::vector<double> len = std::vector<double>(polylines_copy.size());
    //     std::vector<size_t> ids(len.size());
    //     std::iota(begin(ids), end(ids), 0);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Get bounding box length of polylines_copy
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     CGAL_Polyline diagonal;
    //     for (auto i = 0; i < polylines_copy.size(); i++)
    //     {
    //         CGAL::Bbox_3 AABB = CGAL::bbox_3(polylines_copy[i].begin(), polylines_copy[i].end(), IK());
    //         auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
    //         auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
    //         len[i] = std::sqrt(CGAL::squared_distance(p0, p1));
    //     }

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Sort polylines_copy indices by the bounding box length
    //     // WARNING orientation is not checked, which should be fine with the Polylabel algorithm
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     std::sort(begin(ids), end(ids), [&](size_t ia, size_t ib)
    //               { return len[ia] < len[ib]; });
    //     std::reverse(begin(ids), end(ids));

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Convert polylines_copy to Polylabel datastructure, be aware that the polyline considers all polygons closed, so skip the last point
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     mapbox::geometry::polygon<double> polygon_with_holes(polylines_copy.size());

    //     for (auto i = 0; i < polylines_copy.size(); i++)
    //         for (auto j = 0; j < polylines_copy[ids[i]].size() - 1; j++)
    //             polygon_with_holes[i].emplace_back(polylines_copy[ids[i]][j].x(), polylines_copy[ids[i]][j].y());

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Run the Polylabel algorithm, WARNING numbers are in 2D, first two doubles are center, the third is the radius
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     std::array<double, 3> center_and_radius = mapbox::polylabel(polygon_with_holes, precision);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Find closest edge direction to the center
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     IK::Point_3 center(center_and_radius[0], center_and_radius[1], 0);
    //     double distance_from_center_to_polyline_edge = DBL_MAX;
    //     size_t edge_id0 = 0;
    //     size_t edge_id1 = 0;
    //     for (auto i = 0; i < polylines_copy.size(); i++)
    //     {
    //         for (auto j = 0; j < polylines_copy[i].size() - 1; j++)
    //         {

    //             IK::Segment_3 polyline_segment(polylines_copy[i][j], polylines_copy[i][j + 1]);
    //             IK::Point_3 projection = polyline_segment.supporting_line().projection(center);

    //             // closest point paraneters
    //             bool has_on = false;
    //             const IK::Vector_3 polyline_segment_vector = polyline_segment.to_vector();
    //             const double polyline_segment_vector_squared_len = polyline_segment_vector.squared_length();
    //             double t = 0;

    //             if (polyline_segment_vector.squared_length() > 0.0)
    //             {
    //                 if ((projection - polyline_segment[0]).squared_length() <= (projection - polyline_segment[1]).squared_length())
    //                     t = ((projection - polyline_segment[0]) * polyline_segment_vector) / polyline_segment_vector_squared_len;
    //                 else
    //                     t = 1.0 + ((projection - polyline_segment[1]) * polyline_segment_vector) / polyline_segment_vector_squared_len;

    //                 has_on = t >= 0 && t <= 1;
    //             }
    //             else
    //             { // (GBA) Closest point to a degenerate line works as well
    //                 t = 0.0;
    //                 has_on = false;
    //             }

    //             if (has_on)
    //             {
    //                 double temp_distance_from_center_to_polyline_edge = CGAL::squared_distance(center, projection);
    //                 if (temp_distance_from_center_to_polyline_edge < distance_from_center_to_polyline_edge)
    //                 {
    //                     distance_from_center_to_polyline_edge = temp_distance_from_center_to_polyline_edge;
    //                     edge_id0 = i;
    //                     edge_id1 = j;
    //                 }
    //             }
    //         }
    //     }
    //     std::cout << edge_id0 << " " << edge_id1 << "\n";

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Direction
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     IK::Vector_3 x_axis = polylines_copy[edge_id0][edge_id1 + 1] - polylines_copy[edge_id0][edge_id1];
    //     double angle = 0.785398163397448309616; // 1.57079632679489661923;
    //     // angle *= 0;
    //     x_axis = IK::Vector_3(x_axis.hx() * std::cos(angle) - x_axis.hy() * std::sin(angle), x_axis.hx() * std::sin(angle) + x_axis.hy() * std::cos(angle), 0);
    //     cgal_vector_util::Unitize(x_axis);

    //     IK::Vector_3 z_axis(0, 0, 1);
    //     IK::Vector_3 y_axis = CGAL::cross_product(x_axis, z_axis);

    //     // run the division method which is the output -> points
    //     std::vector<IK::Point_3> four_points;
    //     internal::circle_points(IK::Vector_3(center_and_radius[0], center_and_radius[1], 0), x_axis, y_axis, z_axis, four_points, 4, std::get<2>(center_and_radius));
    //     four_points.emplace_back(four_points.front());
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Draw two diagoanals, the length of the x,y axes are equal to the biggest bounding-rectangle diagonal length
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // IK::Vector_2 x_axis(len[ids[0]], 0);
    //     // IK::Vector_2 y_axis(0, len[ids[0]]);

    //     // double angle = 1.57079632679489661923;
    //     // IK::Vector_3 y_axis(x_axis.hx() * std::cos(angle) - x_axis.hy() * std::sin(angle), x_axis.hx() * std::sin(angle) + x_axis.hy() * std::cos(angle), 0);
    //     x_axis *= len[ids[0]];
    //     y_axis *= len[ids[0]];
    //     Clipper2Lib::Paths64 segment0{{Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] - x_axis.hx() - y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] - x_axis.hy() - y_axis.hy()))),
    //                                    Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] + x_axis.hx() + y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] + x_axis.hy() + y_axis.hy())))}};

    //     Clipper2Lib::Paths64 segment1{{Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] + x_axis.hx() - y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] + x_axis.hy() - y_axis.hy()))),
    //                                    Clipper2Lib::Point64((int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[0] - x_axis.hx() + y_axis.hx())), (int64_t)(wood_globals::CLIPPER_SCALE * (center_and_radius[1] - x_axis.hy() + y_axis.hy())))}};

    //     Clipper2Lib::Paths64 paths;
    //     clipper_util::internal::cgalpolylines_to_clipper_2d(polylines_copy, paths);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Intersect diagonals with all the polygons
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     Clipper2Lib::Clipper64 c0;
    //     c0.AddOpenSubject(segment0);
    //     c0.AddClip(paths);
    //     Clipper2Lib::Paths64 soln0, soln_open0;
    //     c0.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::NonZero, soln0, soln_open0);

    //     Clipper2Lib::Clipper64 c1;
    //     c1.AddOpenSubject(segment1);
    //     c1.AddClip(paths);
    //     Clipper2Lib::Paths64 soln1, soln_open1;
    //     c1.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::NonZero, soln1, soln_open1);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Find clipper cut lines closest to the center and select them
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     if (soln_open0.size() == 0 || soln_open1.size() == 0)
    //         return false;

    //     Clipper2Lib::Point64 center_clipper(
    //         (int64_t)(wood_globals::CLIPPER_SCALE * center_and_radius[0]),
    //         (int64_t)(wood_globals::CLIPPER_SCALE * center_and_radius[1]));

    //     int64_t squared_distance_to_center = INT64_MAX;
    //     size_t id0 = 0;
    //     double min_radius;
    //     for (size_t i = 0; i < soln_open0.size(); i++)
    //     {
    //         int64_t temp_squared_distance_to_center = std::pow((center_clipper.x - soln_open0[i][0].x), 2) + std::pow((center_clipper.y - soln_open0[i][0].y), 2);
    //         if (temp_squared_distance_to_center < squared_distance_to_center)
    //         {
    //             // std::cout << squared_distance_to_center << " " << temp_squared_distance_to_center << " " << soln_open0.size() << "\n";
    //             squared_distance_to_center = temp_squared_distance_to_center;
    //             id0 = i;
    //         }
    //     }

    //     squared_distance_to_center = INT64_MAX;
    //     size_t id1 = 0;
    //     for (size_t i = 0; i < soln_open1.size(); i++)
    //     {

    //         int64_t temp_squared_distance_to_center = std::pow((center_clipper.x - soln_open1[i][0].x), 2) + std::pow((center_clipper.y - soln_open1[i][0].y), 2);
    //         if (temp_squared_distance_to_center < squared_distance_to_center)
    //         {
    //             // std::cout << squared_distance_to_center << " " << temp_squared_distance_to_center << " " << soln_open1.size() << "\n";
    //             squared_distance_to_center = temp_squared_distance_to_center;
    //             id1 = i;
    //         }
    //     }
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Find minimal radius
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // for (auto &point)
    //     //  Clipper2Lib::Paths64 segment0_clipped = Clipper2Lib::Intersect(paths, segment0, Clipper2Lib::FillRule::NonZero);
    //     //  Clipper2Lib::Paths64 segment1_clipped = Clipper2Lib::Intersect(paths, segment1, Clipper2Lib::FillRule::NonZero);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Step 6 - Convert to CGAL polyline and rotate to axis and orient to 3D
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     CGAL_Polyline segment0_clipped, segment1_clipped;
    //     CGAL::Aff_transformation_3<IK> identity = CGAL::Identity_transformation();
    //     clipper_util::internal::clipper_to_cgalpolyline_3d(soln_open0[id0], identity, segment0_clipped, false); // xform_toXY_Inv
    //     clipper_util::internal::clipper_to_cgalpolyline_3d(soln_open1[id1], identity, segment1_clipped, false); // xform_toXY_Inv
    //                                                                                                             // clipper_util::internal::clipper_to_cgalpolyline_3d(segment0[0], identity, segment0_clipped, false); // xform_toXY_Inv
    //                                                                                                             // clipper_util::internal::clipper_to_cgalpolyline_3d(segment1[0], identity, segment1_clipped, false); // xform_toXY_Inv

    //     CGAL_Polyline seg = {polylines_copy[edge_id0][edge_id1 + 1], polylines_copy[edge_id0][edge_id1]};
    //     result = {
    //         seg, //
    //         polylines_copy[0],
    //         segment0_clipped,
    //         segment1_clipped,
    //         four_points};

    //     return true;
    // }
}