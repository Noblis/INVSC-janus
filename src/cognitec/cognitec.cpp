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
#include <FRsdk/portrait.h>
#include <FRsdk/eyes.h>

using namespace std;
using namespace FRsdk;

class JanusBody: ImageBody
{
public:
  JanusBody(janus_data* data, size_t width, size_t height, size_t step, janus_color_space color_space)
  {
	  h = static_cast<unsigned int>(height);
	  str = static_cast<unsigned int>(step);
	  w = static_cast<unsigned int>(width);

	  if(color_space == JANUS_BGR24)
	  {
		  cR = (Rgb*)data;
		  c = true;
	  }
	  else
	  {
		  gR = (Byte*)data;
		  c = false;
	  }
  }

  bool isColor() {return c;}

  unsigned int width() const {return w;}

  unsigned int height() const {return h;}

  const Byte* grayScaleRepresentation() const {if (!c) {return gR;} else return NULL;}

  const Rgb* colorRepresentation() const {if(c){return cR;} else return NULL;}

  unsigned int stride() const {return str;}

  std::string name() const {return "";}

private:
  bool c;
  unsigned int w;
  unsigned int h;
  const Byte* gR;
  const Rgb* cR;
  unsigned int str;
};

struct sort_first_greater {
    bool operator()(const std::pair<float,janus_template_id> &left, const std::pair<float,janus_template_id> &right) {
        return left.first > right.first;
    }
    bool operator()(const std::pair<float,FRsdk::Portrait::Characteristics> &left, const std::pair<float,FRsdk::Portrait::Characteristics> &right) {
        return left.first > right.first;
    }
};

Configuration *config;
Face::Finder *face_detector;
Eyes::Finder *eye_detector;
Portrait::Analyzer *portrait_analyzer;

void to_cognitec_image(janus_media media, vector<CountedPtr<JanusBody>> &image_bodies)
{
	for (size_t i = 0; i< media.data.size();i++)
	{
		JanusBody body = new JanusBody(media.data[0],media.width,media.height,media.step,media.color_space);
		image_bodies.push_back(CountedPtr<JanusBody>(&body));
	}
}

janus_error janus_initialize(const string &sdk_path, const string &, const string &algorithm, const int)
{
	config = new Configuration(algorithm);

	face_detector = new Face::Finder(*config);
	eye_detector = new Eyes::Finder(*config);
	portrait_analyzer = new Portrait::Analyzer(*config);

	return JANUS_SUCCESS;
}
janus_error janus_detect(const janus_media &media, const size_t min_face_size, std::vector<janus_track> &tracks)
{
	vector<FRsdk::CountedPtr<JanusBody>> image_bodies;
	to_cognitec_image(media,image_bodies);

	for (size_t i = 0; i< image_bodies.size();i++)
	{
		//Find interpupillary markers in the image
		Face::LocationSet locations = face_detector->find(image_bodies[i]);
        Face::LocationSet::const_iterator faceIter = locations.begin();

        vector<pair<float,FRsdk::Portrait::Characteristics> > face_confidences;
        while( faceIter != locations.end())
        {
        		// Detect the eyes in the face locations
        		Eyes::LocationSet eye_locations = eye_detector->find(image_bodies[i],*faceIter);

        		//Choose the eyes with the highest confidence
        		Eyes::Location &eye_location = eye_locations.front();

        		//Create the image annotated with the eye information
			AnnotatedImage annotated (image_bodies[i],eye_location);

			//Get the face characteristics from the annotated image
			FRsdk::Portrait::Characteristics face_metadata = portrait_analyzer->analyze(annotated);

			face_confidences.push_back(make_pair(faceIter->confidence,face_metadata));
        		faceIter++;
        }

        sort(face_confidences.begin(), face_confidences.end(), sort_first_greater());

        for (size_t j=0; j< face_confidences.size();j++)
        {
        		janus_track track;
        		janus_attributes attributes;

        		attributes.face_height = face_confidences[j].second.height();
        		attributes.face_width = face_confidences[j].second.width();

			if (attributes.face_width < (int)min_face_size)
				continue;

        		attributes.face_x = face_confidences[j].second.faceCenter().x() - (attributes.face_width/2);
        		attributes.face_y = face_confidences[j].second.faceCenter().y() - (attributes.face_height/2);

        		track.track.push_back(attributes);
        		track.detection_confidence = face_confidences[j].first;
        		tracks.push_back(track);
        }
	}

	return JANUS_SUCCESS;
}
