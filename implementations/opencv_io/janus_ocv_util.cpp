#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <iomanip>
#include <map>

#include <opencv_io.h>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef bool (*operation)(janus_media_type &, const vector<string> &, const string &);

// operation_name : { help string, fn pointer }
map<string, pair<string, operation>> operation_map;
#define OP_REGISTER(name, help, fn) operation_map.emplace(name, pair<string,operation>(help, fn));

// DEFINE ALL OPERATIONS HERE

// DUMP FRAMES
bool dump_frames(janus_media_type &media, const vector<string> &args, const string &out)
{
    Mat tmp;
    
    int start = 0;
    int skip = 1;
    int stop = media.frames;
    int n_dumped = 0;
    
    switch (args.size()) {
        case 0: break;

        case 1: start = stoi(args[0]); break;

        case 2: start = stoi(args[0]); 
                stop  = stoi(args[1]); break;

        case 3: start = stoi(args[0]); 
                skip  = stoi(args[1]); 
                stop  = stoi(args[2]); break;

        default: return false;
    }
    stop = min(stop, (int) media.frames);

    if (!media.seek(start)) {
        cerr << "Start index seek failed" << endl;
        return false;
    }

    for (int i = start; i < stop; i += skip, n_dumped++) {
        stringstream fn;

        if (!media.get_frame(tmp, i))
            return false;

        fn << out << "/" << setfill('0') << setw(5) << i << ".jpg";
        imwrite(fn.str(), tmp);
    }

    cout << "Dumped " << n_dumped << " frames from " << media.filename << endl;

    return true;
}

// FIND CLOSEST FRAME
bool mse_less(const pair<uint32_t, double>& f, const pair<uint32_t, double>& s)
{
    return f.second < s.second;
}

bool mmse_frame(janus_media_type &media, const vector<string> &args, const string &out)
{
    Mat tmp;
    Mat compare;
    vector<pair<uint32_t, double>> frame_mse;
    double mmse;
    uint32_t frame;
    
    string compare_filename;
    switch (args.size()) {
        case 1: compare_filename = string(args[0]); break;
        default: return false;
    }

    janus_media_type compare_media(compare_filename);
    compare_media.next(compare);

    for (int i = 0; i < media.frames; i++) {
        Mat tmp2;
        double mse;

        if (!media.get_frame(tmp, i))
            break;
        absdiff(compare, tmp, tmp2); // |x1 - x2|
        pow(tmp2, 2.0, tmp2);        // |x1 - x2| .^ 2
        mse = mean(tmp2)[0];         // mean square error
        frame_mse.push_back(pair<uint32_t, double>(i, mse));
    }

    if (frame_mse.size() > 0) {
        sort(frame_mse.begin(), frame_mse.end(), mse_less);

        frame = frame_mse[0].first;
        mmse  = frame_mse[0].second;

        cout << media.filename << "\t" << compare_filename << "\t" << frame << "\t" << mmse << endl;

        for (int i = 1; i < frame_mse.size(); i++) {
            if ((frame_mse[i].second - mmse) > 1e-6) break;
            cout << media.filename      << "\t"
                 << compare_filename    << "\t"
                 << frame_mse[i].first  << "\t"
                 << frame_mse[i].second << endl;
        }
    }

    return true;
}

// END DEFINE OPERATIONS

void register_all()
{
    // register DUMPFRAMES
    OP_REGISTER("DUMPFRAMES", "Provide no arguments to dump all frames in video: DUMPFRAMES\n"
                              "Provide 1 argument to dump all frames >= arg: DUMPFRAMES,5\n"
                              "Provide 2 arguments to dump all frames in a range: DUMPFRAMES,0,100\n"
                              "Provide 3 arguments to dump every n'th frame in a range: DUMPFRAMES,5,5,35", dump_frames);

    // register CLOSESTFRAMES
    OP_REGISTER("CLOSESTFRAMES", "Provide compare filename as only argument: CLOSESTFRAMES,/path/to/compare\n", mmse_frame);
}

int main(int argc, char **argv)
{
    string file_in;
    string operation_args;
    string operation_tmp;
    string operation;
    string output;
    vector<string> args;
    stringstream arg_stream;
    const string EMPTY_STR("");

    // register all operations
    register_all();

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <FILE NAME> <OPERATION> [OUTPUT]" << endl;
        cerr << "  Some operations may accept arguments.  Arguments are separated with ,'s." << endl;
        cerr << "  For help with an operation, invoke <OPERATION>,?" << endl;
        cerr << "Available operations:" << endl;
        for (auto iter= operation_map.begin() ; iter != operation_map.end() ; iter++)
            cerr << " " << iter->first << endl;
        return JANUS_UNKNOWN_ERROR;
    }

    file_in = string(argv[1]);
    operation_args = string(argv[2]);
    output = (argc < 4) ? EMPTY_STR : string(argv[3]);

    janus_media_type media(file_in);

    // parse args
    arg_stream << operation_args;
    getline(arg_stream, operation, ',');

    while (getline(arg_stream, operation_tmp, ','))
        args.push_back(string(operation_tmp));

    if (!media.valid()) {
        cerr << file_in << " is not valid" << endl;
        return JANUS_INVALID_MEDIA;
    }

    auto iter = operation_map.find(operation);

    if (iter == operation_map.end()) {
        cerr << "No operation: " << operation << endl;
        return JANUS_UNKNOWN_ERROR;
    }
   
    // call function or show usage instructions
    bool op_stat = (operation_args.find('?') == string::npos) ? (*iter->second.second)(media, args, output) : false;

    if (!op_stat) {
        string &help = iter->second.first;
        cerr << "Operation: " << operation << " failed.  Usage:" << endl << help << endl;
    }

    return op_stat;
}
