#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <cmath>
// add some other header files you need

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;
    // std::clog << "view" << std::endl << view << std::endl;  // check data
    return view;
}


Eigen::Matrix4f get_M_T(Eigen::Vector3f T) {
    Eigen::Matrix4f M_trans = Eigen::Matrix4f::Identity();
    M_trans.col(3).head(3) = T;
    return M_trans;
}
Eigen::Matrix4f get_M_S(Eigen::Vector3f S) {
    Eigen::Matrix4f S_trans = Eigen::Matrix4f::Identity();
    Eigen::Matrix3f S_diag = S.asDiagonal();
    S_trans.block(0, 0, 3, 3) = S_diag;
    return S_trans;
}
Eigen::Matrix4f get_M_R(Eigen::Vector3f p0, Eigen::Vector3f u, float rot_angle) {
    u.normalize();
    Eigen::Matrix4f delta_p = get_M_T(p0);
    float radian = (rot_angle*MY_PI) / 180;
	Eigen::Matrix3f R;
    Eigen::Matrix4f res = Eigen::Matrix4f::Identity();
    Eigen::Matrix3f N;
    N << 0, -u[2], u[1],
         u[2], 0, -u[0],
         -u[1], u[0], 0;
    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
    R = cosf(radian) * I + (1 - cosf(radian)) * u*u.transpose() + sinf(radian) * N;
	// Eigen::AngleAxisf rotation_vector(rot_angle/180 * MY_PI, u);
	// rotation_matrix = rotation_vector.toRotationMatrix();
    res.block(0, 0, 3, 3) = R;
    return delta_p * res * get_M_T(Eigen::Vector3f(-p0.x(), -p0.y(), -p0.z()));
}

Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector3f T, Eigen::Vector3f S, Eigen::Vector3f P0, Eigen::Vector3f P1) {
    Eigen::Matrix4f M_trans = get_M_T(T);// get translation matrix
    Eigen::Matrix4f S_trans = get_M_S(S);// get scaling matrix
    Eigen::Matrix4f R_trans = get_M_R(P0, P1-P0, rotation_angle);// get rotation matrix
    Eigen::Matrix4f model = R_trans * S_trans * M_trans;// mix the transformations
	return model;
}

Eigen::Matrix4f squeeze_frustum(float N, float F) {
    Eigen::Matrix4f res;
    res << N, 0, 0, 0,
           0, N, 0, 0,
           0, 0, N+F, -N*F,
           0, 0, 1, 0;
    return res;
}
Eigen::Matrix4f orthoProject(float n, float f, float l, float r, float t, float b) {
    Eigen::Matrix4f moveToOri = Eigen::Matrix4f::Identity();
    moveToOri.col(3) = Eigen::Vector4f(-(r+l)/2, -(t+b)/2, -(n+f)/2, 1);
    Eigen::Matrix4f cubic;
    cubic = Eigen::Vector4f(2/(r-l), 2/(t-b), 2/(n-f), 1).asDiagonal();// right-hand coordinates
    return cubic * moveToOri;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Implement this function

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    float n, f, l, r, t, b;
    n = zNear; f = zFar;
    t = -n * tanf(eye_fov/2); b = -t;
    r = t * aspect_ratio; l = -r;

    // frustum -> cubic
    Eigen::Matrix4f M_squeeze = squeeze_frustum(zNear, zFar);
    // orthographic projection
    Eigen::Matrix4f M_orthoProject = orthoProject(n, f, l, r, t, b);
    // squash all transformations
    Eigen::Matrix4f projection = M_orthoProject * M_squeeze;
    // std::clog << "projection" << std::endl << projection << std::endl; //check
    return projection;
}

float* prompt_for_vertex(int ind) {
    float px, py, pz, *res = new float[4];
    char t = '0';
    std::cout << "You can now set a vertex in the 3D right-hand coordinate system!" << std::endl;
    std::cout << "set the x coordinate: ";
    std::cin >> px;
    std::cout << "set the y coordinate: ";
    std::cin >> py;
    std::cout << "set the z coordinate: ";
    std::cin >> pz;
    std::cout << "your vertex of index " << ind << " is (" << px << " " << py << " " << pz << ")." << std::endl;
    while (t != 'y' && t != 'n') {
        std::cout << "is this your last vertex? (y/n): ";
        std::cin >> t;
    }
    res[0]=px, res[1]=py, res[2] = pz, res[3] = (t == 'y' ? 1 : 0);
    return res; // the last element stands for "whether the input ends".
}
int* prompt_for_index(int maxind) {
    int p1=-1, p2=-1, p3=-1, *res = new int[4];
    char t = '0';
    std::cout << "You can now choose 3 vertices to draw a triangle! (you have indices " << 0 << " - " << maxind << ")" << std::endl;
    while (p1 < 0 || p1 > maxind) {
        std::cout << "Enter the FIRST index of vertex you choose: ";
        std::cin >> p1;
    }
    while (p2 < 0 || p2 > maxind) {
        std::cout << "Enter the SECOND index of vertex you choose: ";
        std::cin >> p2;
    }
    while (p3 < 0 || p3 > maxind) {
        std::cout << "Enter the THIRD index of vertex you choose: ";
        std::cin >> p3;
    }
    std::cout << "the vertices of the triangle are: " << p1 << " - " << p2 << " - " << p3 << std::endl;
    while (t != 'y' && t != 'n') {
        std::cout << "is this your last triangle? (y/n): ";
        std::cin >> t;
    }
    res[0]=p1, res[1]=p2, res[2] = p3, res[3] = (t == 'y' ? 1 : 0);
    return res; // the last element stands for "whether the input ends".
}

int main(int argc, const char** argv) // argv = {Ass2, -G/-I, (filename,) <angle>, 
                                      //         <p0_x>, <p0_y>, <p0_z>, <p1_x>, <p1_y>, <p1_z>}
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "result.png";
    Eigen::Vector3f P0 = Eigen::Vector3f(0, 0, 0);
    Eigen::Vector3f P1 = Eigen::Vector3f(0, 1, 0);

    if (argc >= 2) {
        if (std::string(argv[1]) == "-I") {
            command_line = true;
            if (argc == 2) {
                std::cout << "Error: too few arguments, expected filename." << std::endl;
                return 0;
            }
            filename = std::string(argv[2]);
            if (argc >= 4) {
                angle = std::stof(argv[3]); // -r by default
                if (argc >= 5) {
                    if (argc != 10) {
                        std::cout << "Error: too few arguments. Expected coordinates for P0 and P1." << std::endl;
                        return 0;
                    }
                    P0.x() = std::stof(argv[4]), P0.y() = std::stof(argv[5]), P0.z() = std::stof(argv[6]);
                    P1.x() = std::stof(argv[7]), P1.y() = std::stof(argv[8]), P1.z() = std::stof(argv[9]);
                }
            }
        }
        else if (std::string(argv[1]) == "-G") {
            if (argc >= 3) {
                angle = std::stof(argv[2]);
                if (argc >= 4) {
                    if (argc != 9) {
                        std::cout << "Error: too few arguments. Expected all coordinates for P0 and P1." << std::endl;
                        return 0;
                    }
                    P0.x() = std::stof(argv[3]), P0.y() = std::stof(argv[4]), P0.z() = std::stof(argv[5]);
                    P1.x() = std::stof(argv[6]), P1.y() = std::stof(argv[7]), P1.z() = std::stof(argv[8]);
                }
            }
        }
        else {
            std::cout << "Error: unrecognizable instruction " << argv[1] << std::endl;
            return 0;
        }
    }

    rst::rasterizer r(1024, 1024);
    // define your eye position "eye_pos" to a proper position
    Eigen::Vector3f eye_pos = Eigen::Vector3f(0, 0, 10);
    // define a triangle named by "pos" and "ind"
    std::vector<Eigen::Vector3f> pos;
    std::vector<Eigen::Vector3i> ind;
    

    //It is supported to let the user input vertices and draw triangles
    int vcnt = -1;
    char willing = '0';
    while (willing != 'y' && willing != 'n') {
        std::cout << "Would you like to manually set vertices and triangles? (y/n): ";
        std::cin >> willing;
    }
    if (willing == 'y') {
        vcnt = 0;
        float* p_input = prompt_for_vertex(0);//container for vertices
        while (p_input[3] != 1) {
            pos.push_back(Eigen::Vector3f(p_input[0], p_input[1], p_input[2]));
            vcnt++;
            delete[] p_input;
            p_input = prompt_for_vertex(vcnt);
        }
        pos.push_back(Eigen::Vector3f(p_input[0], p_input[1], p_input[2]));
        delete[] p_input;
        if (vcnt < 2) {
            std::cout << "Error: too few vertices, expected at least 3" << std::endl;
            return 0;
        }
        int* i_input = prompt_for_index(vcnt);//container for index groups
        while (i_input[3] != 1) {
            ind.push_back(Eigen::Vector3i(i_input[0], i_input[1], i_input[2]));
            delete[] i_input;
            i_input = prompt_for_index(vcnt);
        }
        ind.push_back(Eigen::Vector3i(i_input[0], i_input[1], i_input[2]));
        delete[] i_input;
    }


    if (vcnt == -1) {// if no user input
        pos.push_back(Eigen::Vector3f(-1, 0, -5));
        pos.push_back(Eigen::Vector3f(1, 0, -5));
        pos.push_back(Eigen::Vector3f(0, 2, -5));
        // pos.push_back(Eigen::Vector3f(2, -1, -3));
        // pos.push_back(Eigen::Vector3f(-1, -1, -3));
        // pos.push_back(Eigen::Vector3f(1, 0, -3));

        ind.push_back(Eigen::Vector3i(0, 1, 2));
        // ind.push_back(Eigen::Vector3i(3, 4, 5));
    }

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);
    
    int key = 0; // key pressed by user
    int frame_count = 0;

    Eigen::Vector3f T = Eigen::Vector3f(0, 0, 0);
    Eigen::Vector3f S = Eigen::Vector3f(1, 1, 1);

    // added parameters for get_projection_matrix(float eye_fov, float aspect_ratio,float zNear, float zFar)
    float eye_fov = MY_PI/2;
    float aspect_ratio = 1;
    float zNear = -1, zFar = 7;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(std::string("../") + filename, image);
        std::cout << "Photo generationg complete!" << std::endl;
        return 0;
    }

    while (key != 27) { // esc = 27 in ASCII
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle, T, S, P0, P1));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(eye_fov, aspect_ratio, zNear, zFar));
        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(1024, 1024, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';
        std::clog << "angle: " << angle << std::endl;
    

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
