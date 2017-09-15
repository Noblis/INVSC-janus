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
#include <pittpatt_nc_sdk.h>
#include <pittpatt_errors.h>
#include <pittpatt_license.h>
#include <iarpa_janus.h>
#include <iarpa_janus_io.h>

using namespace std;

ppr_context_type ppr_context;

struct janus_template_type
{
    ppr_gallery_type ppr_gallery;
};

struct janus_gallery_type
{
	vector<pair<janus_template_id,ppr_gallery_type>> ppr_galleries;
};

static janus_error to_janus_error(ppr_error_type error)
{
    if (error != PPR_SUCCESS)
        printf("PittPatt 4: %d: %s\n", error,ppr_error_message(error));

    switch (error) {
      case PPR_SUCCESS:                 return JANUS_SUCCESS;
      case PPR_INVALID_PATH:     return JANUS_INVALID_SDK_PATH;
      case PPR_NULL_IMAGE:
      case PPR_INVALID_IMAGE:
      case PPR_INCONSISTENT_IMAGE_DIMENSIONS:
                                        return JANUS_INVALID_MEDIA;
      default:                          return JANUS_UNKNOWN_ERROR;
    }
}

#define JANUS_TRY_PPR(PPR_API_CALL)            \
{                                              \
    ppr_error_type ppr_error = (PPR_API_CALL); \
    if (ppr_error != PPR_SUCCESS)              \
        return to_janus_error(ppr_error);      \
}

#define TRY(PPR_API_CALL)\
{\
    ppr_error_type ppr_error = (PPR_API_CALL);\
    if (ppr_error != PPR_SUCCESS)\
		printf("PittPatt 4: %s\n", ppr_error_message(ppr_error));\
}

static ppr_error_type initialize_ppr_context(ppr_context_type *context, const string *path)
{
    //TODO: Find way to change the following to match PP5 wrapper settings:
    //num_visual_instances to 0
    //extract_thumbnails off

	TRY(ppr_set_license(*context,my_license_id,my_license_key));

    //General parameters
    TRY(ppr_set_models_path(*context,(*path).c_str()));

    //Detection parameters
    //Calibrate detection threshold?
	TRY(ppr_set_detection_precision(*context,PPR_FINE_PRECISION));
    TRY(ppr_set_landmark_detector_type(*context,PPR_DUAL_MULTI_POSE_LANDMARK_DETECTOR,PPR_AUTOMATIC_LANDMARKS));
    TRY(ppr_set_num_detection_threads(*context,1));
    TRY(ppr_set_frontal_yaw_constraint(*context,PPR_FRONTAL_YAW_CONSTRAINT_PERMISSIVE));
    TRY(ppr_set_search_pruning_aggressiveness(*context,0));
    TRY(ppr_set_min_size(*context,PPR_MIN_MIN_SIZE));
    TRY(ppr_set_max_size(*context,PPR_MAX_MAX_SIZE));
    TRY(ppr_set_adaptive_min_size(*context,.01f));
    TRY(ppr_set_adaptive_max_size(*context,1.f));

    //Recognition parameters
    to_janus_error(ppr_enable_recognition(*context));
    to_janus_error(ppr_set_num_recognition_threads(*context,1));

    return ppr_initialize_context(*context);
}

janus_error janus_initialize(const string &sdk_path, const string &, const string &, const int)
{
    ppr_context = ppr_get_context();
    return to_janus_error(initialize_ppr_context(&ppr_context,&sdk_path));
}

janus_error janus_finalize()
{
    janus_error error = to_janus_error(ppr_release_context(ppr_context));
    ppr_finalize_sdk();
    return error;
}

static ppr_error_type to_ppr_media(const janus_media &media, vector<ppr_image_type> &ppr_media)
{
    for (size_t i = 0; i < media.data.size(); i++) {
        ppr_raw_image_type raw_image;
        raw_image.bytes_per_line = (media.color_space == JANUS_BGR24 ? 3 : 1) * media.width;
        raw_image.color_space = (media.color_space == JANUS_BGR24 ? PPR_RAW_IMAGE_BGR24 : PPR_RAW_IMAGE_GRAY8);
        raw_image.data = media.data[i];
        raw_image.height = media.height;
        raw_image.width = media.width;

        ppr_image_type ppr_image;
        ppr_error_type error = ppr_create_image(raw_image, &ppr_image);
        if (error != PPR_SUCCESS)
            return error;
        ppr_media.push_back(ppr_image);
    }

    return PPR_SUCCESS;
}

struct sort_first_greater {
    bool operator()(const std::pair<double,janus_template_id> &left, const std::pair<double,janus_template_id> &right) {
        return left.first > right.first;
    }
    bool operator()(const std::pair<float,ppr_object_type> &left, const std::pair<float,ppr_object_type> &right) {
        return left.first > right.first;
    }
};

janus_error janus_detect(const janus_media &media, const size_t min_face_size, std::vector<janus_track> &tracks)
{
    vector<ppr_image_type> ppr_media;
    JANUS_TRY_PPR(to_ppr_media(media, ppr_media))

    ppr_context_type *context = &ppr_context;

    for (size_t i = 0; i < ppr_media.size(); i++) {
        ppr_object_list_type object_list;
        ppr_detect_objects(*context, ppr_media[i], &object_list);

        vector<pair<float, ppr_object_type> > face_confidences;
        for (int j = 0; j < object_list.num_objects; j++) {
            ppr_object_type object = object_list.objects[j];
            face_confidences.push_back(make_pair(object.confidence, object));
        }

        // Sort by confidence, descending
        sort(face_confidences.begin(), face_confidences.end(), sort_first_greater());

        for (size_t j = 0; j < face_confidences.size(); j++) {
            janus_track track;
            janus_attributes attributes;
            attributes.face_x = (int) (face_confidences[j].second.position.x - face_confidences[j].second.dimensions.width/2.f);
            attributes.face_y = (int) (face_confidences[j].second.position.y - face_confidences[j].second.dimensions.height/2.f);
            attributes.face_width = (int) face_confidences[j].second.dimensions.width;
            if (attributes.face_width < (int)min_face_size)
                continue;
            attributes.face_height = (int) face_confidences[j].second.dimensions.height;
            attributes.frame_number = i;
            track.track.push_back(attributes);
            track.detection_confidence = face_confidences[j].first;
            tracks.push_back(track);
        }

        ppr_free_object_list(object_list);
    }

    for (size_t i = 0; i < ppr_media.size(); i++)
        ppr_free_image(ppr_media[i]);

    return JANUS_SUCCESS;
}


//This is all Janus related things. Not sure what needs changing in here.
janus_media janus_crop_media(const janus_association &association)
{
    const janus_media &src = association.media;
    const janus_track &track = association.metadata;

    janus_media dst; dst.data.reserve(src.data.size());
    for (size_t i = 0; i < src.data.size(); i++) {
        const janus_attributes &face_attributes = track.track[i];
        const size_t x = face_attributes.face_x < 0 ? 0 : face_attributes.face_x;
        const size_t y = face_attributes.face_y < 0 ? 0 : face_attributes.face_y;
        dst.width = (x + face_attributes.face_width > src.width) ? src.width - x : face_attributes.face_width;
        dst.height = (y + face_attributes.face_height > src.height) ? src.height - y : face_attributes.face_height;
        dst.color_space = src.color_space;
        int channels = src.color_space == JANUS_BGR24 ? 3 : 1;

        janus_data *data = new janus_data[dst.width * dst.height * channels];
        const unsigned long dst_elements_per_row = dst.width * channels;
        const unsigned long src_elements_per_row = src.width * channels;

        for (size_t j = 0; j < dst.height; j++)
            memcpy(data + j*dst_elements_per_row, src.data[i] + src_elements_per_row * (y + j) + channels*x, dst_elements_per_row);
        dst.data.push_back(data);
    }
    return dst;
}


janus_error janus_create_template(std::vector<janus_association> &associations, const janus_template_role, janus_template &template_)
{
	int gallery_size=0;
    template_ = new janus_template_type();
    JANUS_TRY_PPR(ppr_create_gallery(ppr_context,&(template_->ppr_gallery)));
    for (size_t i = 0; i < associations.size(); i++) {
        janus_media cropped = janus_crop_media(associations[i]);

        vector<ppr_image_type> ppr_media;
        to_ppr_media(cropped, ppr_media);

        for (size_t j = 0; j < ppr_media.size(); j++) {
            ppr_template_type _template = nullptr;
            ppr_object_list_type object_list;
            JANUS_TRY_PPR(ppr_detect_objects(ppr_context, ppr_media[j], &object_list))

            for (int k = 0; k < object_list.num_objects; k++) {
                ppr_object_type object = object_list.objects[k];
                ppr_object_suitability_type suitability;
                JANUS_TRY_PPR(ppr_is_object_suitable_for_recognition(ppr_context, object, &suitability))
                if (suitability == PPR_OBJECT_SUITABLE_FOR_RECOGNITION) { // Only extract a single face template
                    JANUS_TRY_PPR(ppr_extract_template_from_object(ppr_context, ppr_media[j], object,&_template));
                    break;
                }
            }
            if(!_template)
            {
            		continue;
            }
            else{
				int id; //not needed.
				JANUS_TRY_PPR(ppr_copy_template_to_gallery(ppr_context,&(template_->ppr_gallery),_template,&id));
				ppr_free_object_list(object_list);
				gallery_size++;
            }
        }

        for (size_t i = 0; i < ppr_media.size(); i++)
            ppr_free_image(ppr_media[i]);
        janus_free_media(cropped);
    }
    if(gallery_size == 0){
    		return JANUS_FAILURE_TO_ENROLL;
    }
    for(int i=1;i<gallery_size;i++)
    {
    		JANUS_TRY_PPR(ppr_set_template_relationship(ppr_context,&(template_->ppr_gallery),i-1,i,PPR_RELATIONSHIP_SAME_SUBJECT));
    }

    return JANUS_SUCCESS;
}

janus_error janus_create_template(const janus_media &media, const janus_template_role role, vector<janus_template> &templates, vector<janus_track> &tracks)
{
    vector<janus_track> tmp_tracks;
    janus_error error = janus_detect(media, 20, tmp_tracks);
    
    if (error == JANUS_SUCCESS) {
        for (janus_track &track : tmp_tracks) {
            janus_template tmpl;
            janus_association association;
            association.media = media;
                association.metadata = track;
                
                vector<janus_association> tmp_association = {association};
                error = janus_create_template(tmp_association, role, tmpl);
                if (error == JANUS_SUCCESS) {
                    templates.push_back(tmpl);
                    tracks.push_back(track);
                }
        }
    } else return JANUS_FAILURE_TO_DETECT;
    return JANUS_SUCCESS;
}

janus_error janus_serialize_template(const janus_template &template_, std::ostream &stream) 
{
    size_t bytes = 0;

    ppr_flat_gallery_type flat_data;
    JANUS_TRY_PPR(ppr_flatten_gallery(ppr_context,template_->ppr_gallery,&flat_data));
    bytes+=sizeof(size_t);
    bytes+=flat_data.num_bytes;

    // total bytes (not including itself)
    stream.write(reinterpret_cast<const char *>(&bytes), sizeof(size_t));

    // the template
	stream.write(static_cast<const char *>(flat_data.data), flat_data.num_bytes);

	// free the template data
	ppr_free_flat_gallery(flat_data);

    return JANUS_SUCCESS;
}

janus_error janus_deserialize_template(janus_template &template_, std::istream &stream)
{
	size_t total_bytes = 0;
	template_ = new janus_template_type();
	JANUS_TRY_PPR(ppr_create_gallery(ppr_context,&(template_->ppr_gallery)));

	// total bytes
	stream.read(reinterpret_cast<char *>(&total_bytes), sizeof(size_t));
	total_bytes-=sizeof(size_t);

	if (total_bytes == 0) return JANUS_FAILURE_TO_DESERIALIZE;
	// read template
	ppr_flat_gallery_type *flat_data = new ppr_flat_gallery_type;
	flat_data->num_bytes=total_bytes;
	flat_data->data = malloc(total_bytes);
	stream.read(static_cast<char *>(flat_data->data), total_bytes);
	// add to face list
	JANUS_TRY_PPR(ppr_unflatten_gallery(ppr_context,*flat_data, &(template_->ppr_gallery)));
	ppr_free_flat_gallery(*flat_data);

    return JANUS_SUCCESS;
}

janus_error janus_delete_serialized_template(janus_data *&template_, const size_t)
{
    delete[] template_;
    return JANUS_SUCCESS;
}

janus_error janus_delete_template(janus_template &template_)
{

	ppr_free_gallery(template_->ppr_gallery);
	delete template_;
    return JANUS_SUCCESS;
}

janus_error janus_verify(const janus_template &reference, const janus_template &verification, double &similarity)
{
    // Set the default similarity score to be a rejection score (for galleries that don't contain faces)
    similarity = -1.5;

    ppr_similarity_matrix_type simmat;
    JANUS_TRY_PPR(ppr_compare_galleries(ppr_context, reference->ppr_gallery, verification->ppr_gallery, &simmat))

    ppr_template_id_list_type rows;
    ppr_template_id_list_type columns;

    JANUS_TRY_PPR(ppr_get_query_template_id_list_from_similarity_matrix(ppr_context,simmat,&rows));
    JANUS_TRY_PPR(ppr_get_target_template_id_list_from_similarity_matrix(ppr_context,simmat,&columns));

    int total=0;
    float average=0;
    for(int i=0;i<rows.num_template_ids;i++)
    {
    		for(int j=0;j<columns.num_template_ids;j++)
    		{
    			float score;
    			JANUS_TRY_PPR(ppr_get_similarity_matrix_element(ppr_context,simmat,*(rows.template_ids+i),*(columns.template_ids+j),&score));
    			average+=score;
    			total++;
    		}
    }

    if(total==0)
    {
    		average=-1.5;
    }
    else
    {
    		average /= total;
    }

    similarity = average + 1.5;

    ppr_free_similarity_matrix(simmat);
    ppr_free_template_id_list(rows);
    ppr_free_template_id_list(columns);

    if (similarity != similarity) // True for NaN
        return JANUS_UNKNOWN_ERROR;

    return JANUS_SUCCESS;
}

janus_error janus_create_gallery(const vector<janus_template> &templates, const vector<janus_template_id> &ids, janus_gallery &gallery)
{
    gallery = new janus_gallery_type();

    for (size_t i = 0; i < templates.size(); i++)
    {
    		gallery->ppr_galleries.push_back(make_pair(ids[i],templates[i]->ppr_gallery));
    }
    return JANUS_SUCCESS;
}

janus_error janus_serialize_gallery(const janus_gallery &gallery, std::ostream &stream)
{

	int total_bytes=0;
    vector<pair<janus_template_id,ppr_flat_gallery_type>> data_list;
    for (size_t i=0;i<gallery->ppr_galleries.size();i++)
    {
    		ppr_flat_gallery_type flat_data;
    		janus_template_id &id=gallery->ppr_galleries[i].first;
    		JANUS_TRY_PPR(ppr_flatten_gallery(ppr_context, gallery->ppr_galleries[i].second, &flat_data));
    		total_bytes+=flat_data.num_bytes;
    		total_bytes+=sizeof(size_t); //template ID
    		total_bytes+=sizeof(size_t); //gallery (template) size
    		data_list.push_back(make_pair(id,flat_data));
    }

    //total bytes
    stream.write(reinterpret_cast<const char *>(&total_bytes), sizeof(int));

    for(size_t i=0;i<data_list.size();i++)
    {
    		const ppr_flat_gallery_type &flat_data = data_list[i].second;
    		const janus_template_id &id = data_list[i].first;
    		const size_t template_size = flat_data.num_bytes+sizeof(size_t);

    		//size of the gallery (template) and ID
    		stream.write(reinterpret_cast<const char *>(&template_size), sizeof(size_t));
    		//gallery (template) id and data
    		stream.write(reinterpret_cast<const char *>(&id), sizeof(size_t));
    		stream.write(reinterpret_cast<const char *>(flat_data.data),flat_data.num_bytes);

    		//Free the data
    		ppr_free_flat_gallery(flat_data);

    }
    return JANUS_SUCCESS;
}

janus_error janus_deserialize_gallery(janus_gallery &gallery, std::istream &stream)
{
    int total_bytes=0;
    gallery = new janus_gallery_type();

    //get total size
    stream.read(reinterpret_cast<char *>(&total_bytes), sizeof(int));


    if(total_bytes == 0) return JANUS_FAILURE_TO_DESERIALIZE;

    while(total_bytes>0)
    {
    		ppr_flat_gallery_type *flat_data = new ppr_flat_gallery_type;
    		size_t gallery_bytes;
    		janus_template_id id;

    		stream.read(reinterpret_cast<char *>(&gallery_bytes),sizeof(size_t));
    	    total_bytes-=sizeof(size_t);

    	    stream.read(reinterpret_cast<char *>(&id),sizeof(size_t));
    	    gallery_bytes-=sizeof(size_t);
    	    total_bytes-=sizeof(size_t);

    	    flat_data->data = malloc(gallery_bytes);
    	    flat_data->num_bytes=gallery_bytes;
    	    stream.read(static_cast<char *>(flat_data->data),gallery_bytes);
    	    total_bytes-=gallery_bytes;

    	    ppr_gallery_type newgallery;
    	    JANUS_TRY_PPR(ppr_unflatten_gallery(ppr_context,*flat_data,&newgallery));

    	    gallery->ppr_galleries.push_back(make_pair(id,newgallery));
    	    ppr_free_flat_gallery(*flat_data);
    }

    return JANUS_SUCCESS;
}

janus_error janus_prepare_gallery(janus_gallery &)
{
    return JANUS_SUCCESS;
}

janus_error janus_gallery_insert(janus_gallery &gallery, const janus_template &template_, const janus_template_id &id)
{
	gallery->ppr_galleries.push_back(make_pair(id,template_->ppr_gallery));
	return JANUS_SUCCESS;
}

janus_error janus_gallery_remove(janus_gallery &gallery, const janus_template_id &id)
{
    for(vector<pair<janus_template_id,ppr_gallery_type>>::iterator it = gallery->ppr_galleries.begin(); it != gallery->ppr_galleries.end();it++)
    {
    		if(it->first==id)
    		{
    			ppr_free_gallery(it->second);
    			gallery->ppr_galleries.erase(it);
    			break;
    		}
    }
    return JANUS_SUCCESS;
}
janus_error janus_delete_serialized_gallery(janus_data *&gallery, const size_t)
{
    delete[] gallery;
    return JANUS_SUCCESS;
}

janus_error janus_delete_gallery(janus_gallery &gallery)
{

	delete gallery;
    return JANUS_SUCCESS;
}

janus_error janus_search(const janus_template &probe, const janus_gallery &gallery, const size_t num_requested_returns, vector<janus_template_id> &template_ids, vector<double> &similarities)
{
    vector<pair<double, janus_template_id> > scores;

    janus_template_type *temp = new janus_template_type();

    for(auto &n : gallery->ppr_galleries)
    {
    		double similarity;
    		temp->ppr_gallery=n.second;
    		janus_verify(probe,temp,similarity);
    		scores.push_back(make_pair(similarity,n.first));
    }

    sort(scores.begin(), scores.end(), sort_first_greater());

    const size_t keep = min(scores.size(), num_requested_returns);
    template_ids.reserve(keep); similarities.reserve(keep);
    for (size_t i = 0; i < keep; i++)
    {
        template_ids.push_back(scores[i].second);
        similarities.push_back(scores[i].first);
    }

    return JANUS_SUCCESS;
}

janus_error janus_cluster(const vector<janus_template> &templates,
                          const size_t hint,
                          vector<cluster_pair> &clusters)
{

    // PP4 arguments/data structures
    int clustering_aggressiveness = PPR_MAX_CLUSTERING_AGGRESSIVENESS;
    ppr_inter_gallery_subject_list_type cluster_list;
    ppr_gallery_group_type cluster_group;
    JANUS_TRY_PPR(ppr_create_gallery_group(ppr_context,&cluster_group));

    // janus data structures
    vector<janus_template_id> t_ids(templates.size());

    // make template_ids and make room for cluster pairs
    std::iota(std::begin(t_ids), std::end(t_ids), 0);
    clusters.resize(templates.size());
    std::fill(clusters.begin(), clusters.end(), cluster_pair(-1, -1.5));

    for(size_t i=0; i<templates.size();i++)
    {
    		int galleryid;
    		JANUS_TRY_PPR(ppr_move_gallery_to_gallery_group(ppr_context,&cluster_group,&(templates[i]->ppr_gallery),&galleryid));
    }

    JANUS_TRY_PPR(ppr_cluster_gallery_group(ppr_context,
                                      cluster_group,
                                      clustering_aggressiveness,
                                      &cluster_list));

    for(int cluster_id=0;cluster_id<cluster_list.num_subjects;cluster_id++)
    {
    		ppr_template_source_list_type subject = cluster_list.subjects[cluster_id];
    		for(int j=0;j<subject.num_template_sources;j++)
    		{
    			int t_id=subject.template_sources[j].gallery_id;
    			clusters[t_id].first = cluster_id;
    		}
    }

    // free PP4 structures
    ppr_free_inter_gallery_subject_list(cluster_list);
    ppr_free_gallery_group(cluster_group);

    return JANUS_SUCCESS;
}
