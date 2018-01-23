#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>
#include <numeric>
#include <utility>
#include <unordered_map>
#include <assert.h>
#include <iarpa_janus.h>
#include <iarpa_janus_io.h>

//Cognitec Libraries
#include <FRsdk/config.h>
#include <FRsdk/face.h>

using namespace std;

FRsdk::Configuration config;

janus_error janus_initialize(const string &sdk_path, const string &, const string &algorithm, const int)
{
	config = new FRsdk::Configuration(algorithm);

}
janus_error janus_detect(const janus_media &media, const size_t min_face_size, std::vector<janus_track> &tracks)
{
	FRsdk::Face::Finder detector(config);
}
