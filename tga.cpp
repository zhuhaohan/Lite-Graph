/*
 * tga.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: zhu
 */
#include "timer.hpp"
#include "index.hpp"
#include "covering_snapshot.hpp"
#include "persisting_snapshot.hpp"
#include "existing_snapshot.hpp"
#include "covering_delta.hpp"
#include "persisting_delta.hpp"
#include "existing_delta.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

using namespace std;

int main(){

    int FL_time_array[] = {394563, 1643905, 2883713, 4134935, 5345876, 6543791, 7665540, 8791575, 9707029, 10608420, 11493442};
    int NF_time_array[] = {32004043, 122248287, 131242146, 136468377, 150778804, 156492861, 166278703, 176471905, 195747318, 203198733, 211273538};
    int AS_time_array[] = {248,1442, 2360, 3115, 3660, 4119, 4543, 4843, 5103, 5353, 5533};
    int CPN_time_array[] = {2, 75, 149, 223, 297, 369, 443, 515, 587, 617, 717};
    int CP_time_array[] = {0, 73, 147, 221, 295, 367, 441, 513, 585, 615, 715};
    int TA_time_array[] = {21168000, 39997627, 69002340, 83989380, 99258312, 113601360, 128569680, 142647060, 157772520, 172192393, 187568220};
    int FL_Duration = 11179999;
    int TA_Duration = 168313451;
    int NF_Duration = 181291836;
    int AS_Duration = 5476;
    int CP_Duration = 729;

    int start = FL_time_array[2];
    int end = FL_time_array[3];
    int delta = FL_Duration/10000;

    int E = 120000000;// FL 120000000; TA 800000000; NF 1500000000; AS 5000000; CP 200000000;
    int B = 10000;

    const char* index_file = "/research/datasets/TGA/zhu/FL/FL_INDEX_10K";
    const char* data_file = "/research/datasets/TGA/zhu/FL/FL_PO_10K_B";
    const char* total_data_file = "/research/datasets/TGA/zhu/FL/FL_TO_B";

    Timer build_index_timer;
    build_index_timer.start();
    Index index(index_file, B, E);
    build_index_timer.stop();
    //cout << "Building Bucket Index:\t" << build_index_timer.elapsed_time() << "\t seconds" << endl;

    cout << "*********************************************************" << endl;

    Timer cs_search_bucket_timer;
    cs_search_bucket_timer.start();
    Covering_Snapshot cs(data_file, index, start, end);
    cs_search_bucket_timer.stop();
    //cout << "Covering Snapshot Loading Buckets:\t" << cs_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer cs_retrieve_edge_timer;
    cs_retrieve_edge_timer.start();
    cs.get_duration_pairs();
    cs_retrieve_edge_timer.stop();
    //cout << "Covering Snapshot Identifying Edges: \t" << cs_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer cs_retrieve_result_timer;
    cs_retrieve_result_timer.start();
    cs.retrieve_results();
    cs_retrieve_result_timer.stop();
    //cout << "Covering Snapshot Retrieve Output: \t" << cs_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Covering Snapshot Candidates:\t" << cs.index_max - cs.index_min << endl;
    cout << "# Covering Snapshot Results:\t" << cs.number_results << endl;

    Timer cs_total_search_bucket_timer;
    cs_total_search_bucket_timer.start();
    Covering_Snapshot cst(total_data_file, index, start, end);
    cs_total_search_bucket_timer.stop();
    //cout << "Covering Snapshot Total Edge Stream Loading Buckets:\t" << cs_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer cs_total_edge_stream_timer;
    cs_total_edge_stream_timer.start();
    int xcs = cst.tes();
    cs_total_edge_stream_timer.stop();
    //cout << "Covering Snapshot Total Edge Stream Retrieval: \t" << cs_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Covering Snapshot Results for Total Order: \t" << xcs << endl;
    cout << "CS\t" << cs_retrieve_edge_timer.elapsed_time() << "\t" <<cs_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    /*********************************************************/

    Timer es_search_bucket_timer;
    es_search_bucket_timer.start();
    Existing_Snapshot es(data_file, index, start, end);
    es_search_bucket_timer.stop();
    //cout << "Existing Snapshot Loading Buckets:\t" << es_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer es_retrieve_edge_timer;
    es_retrieve_edge_timer.start();
    es.get_duration_pairs();
    es_retrieve_edge_timer.stop();
    //cout << "Existing Snapshot Identifying Edges: \t" << es_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer es_retrieve_result_timer;
    es_retrieve_result_timer.start();
    es.retrieve_results();
    es_retrieve_result_timer.stop();
    //cout << "Existing Snapshot Retrieve Output: \t" << es_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Existing Snapshot Candidates:\t" << es.index_max - es.index_min << endl;
    cout << "# Existing Snapshot Results:\t" << es.number_results << endl;

    Timer es_total_search_bucket_timer;
    es_total_search_bucket_timer.start();
    Existing_Snapshot est(total_data_file, index, start, end);
    es_total_search_bucket_timer.stop();
    //cout << "Existing Snapshot Total Edge Stream Loading Buckets:\t" << es_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer es_total_edge_stream_timer;
    es_total_edge_stream_timer.start();
    int xes = est.tes();
    es_total_edge_stream_timer.stop();
    //cout << "Existing Snapshot Total Edge Stream Retrieval: \t" << es_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Existing Snapshot Results for Total Order: \t" << xes << endl;
    cout << "ES\t" << es_retrieve_edge_timer.elapsed_time() << "\t" <<es_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    /*********************************************************/

    Timer ps_search_bucket_timer;
    ps_search_bucket_timer.start();
    Persisting_Snapshot ps(data_file, index, start, start+delta);
    ps_search_bucket_timer.stop();
    //cout << "Persisting Snapshot Loading Buckets:\t" << ps_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer ps_retrieve_edge_timer;
    ps_retrieve_edge_timer.start();
    ps.get_duration_pairs();
    ps_retrieve_edge_timer.stop();
    //cout << "Persisting Snapshot Identifying Edges: \t" << ps_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer ps_retrieve_result_timer;
    ps_retrieve_result_timer.start();
    ps.retrieve_results();
    ps_retrieve_result_timer.stop();
    //cout << "Persisting Snapshot Retrieve Output: \t" << ps_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Persisting Snapshot Candidates:\t" << ps.index_max - ps.index_min << endl;
    cout << "# Persisting Snapshot Results:\t" << ps.number_results << endl;

    Timer ps_total_search_bucket_timer;
    ps_total_search_bucket_timer.start();
    Persisting_Snapshot pst(total_data_file, index, start, start+delta);
    ps_total_search_bucket_timer.stop();
    //cout << "Persisting Snapshot Total Edge Stream Loading Buckets:\t" << ps_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer ps_total_edge_stream_timer;
    ps_total_edge_stream_timer.start();
    int xps = pst.tes();
    ps_total_edge_stream_timer.stop();
    //cout << "Persisting Snapshot Total Edge Stream Retrieval: \t" << ps_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Persisting Snapshot Results for Total Order: \t" << xps << endl;
    cout << "PS\t" << ps_retrieve_edge_timer.elapsed_time() << "\t" << ps_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    /*********************************************************/

    Timer cad_search_bucket_timer;
    cad_search_bucket_timer.start();
    Covering_Delta cad(data_file, index, start, end, delta, 1);
    cad_search_bucket_timer.stop();
    //cout << "Covering Adding Duration Loading Buckets:\t" << cad_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer cad_retrieve_edge_timer;
    cad_retrieve_edge_timer.start();
    cad.get_added_duration_pairs();
    cad_retrieve_edge_timer.stop();
    //cout << "Covering Adding Duration Identifying Edges: \t" << cad_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer cad_retrieve_result_timer;
    cad_retrieve_result_timer.start();
    cad.retrieve_added_results();
    cad_retrieve_result_timer.stop();
    //cout << "Covering Adding Duration Retrieve Output: \t" << cad_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Covering Adding Duration Candidates:\t" << cad.index_max - cad.index_min << endl;
    cout << "# Covering Adding Duration Results:\t" << cad.number_added_results << endl;

    Timer cad_total_search_bucket_timer;
    cad_total_search_bucket_timer.start();
    Covering_Delta cadt(total_data_file, index, start, end, delta, 1);
    cad_total_search_bucket_timer.stop();
    //cout << "Covering Adding Duration Total Edge Stream Loading Buckets:\t" << cad_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer cad_total_edge_stream_timer;
    cad_total_edge_stream_timer.start();
    int xcad = cadt.added_tes();
    cad_total_edge_stream_timer.stop();
    //cout << "Covering Adding Duration Total Edge Stream Retrieval: \t" << cad_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Covering Adding Duration Results for Total Order: \t" << xcad << endl;
    cout << "CAD\t" << cad_retrieve_edge_timer.elapsed_time() << "\t" << cad_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    Timer crd_search_bucket_timer;
    crd_search_bucket_timer.start();
    Covering_Delta crd(data_file, index, start, end, delta, 0);
    crd_search_bucket_timer.stop();
    //cout << "Covering Removing Duration Loading Buckets:\t" << crd_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer crd_retrieve_edge_timer;
    crd_retrieve_edge_timer.start();
    crd.get_removed_duration_pairs();
    crd_retrieve_edge_timer.stop();
    //cout << "Covering Removing Duration Identifying Edges: \t" << crd_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer crd_retrieve_result_timer;
    crd_retrieve_result_timer.start();
    crd.retrieve_removed_results();
    crd_retrieve_result_timer.stop();
    //cout << "Covering Removing Duration Retrieve Output: \t" << crd_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Covering Removing Duration Candidates:\t" << crd.index_max - crd.index_min << endl;
    cout << "# Covering Removing Duration Results:\t" << crd.number_removed_results << endl;

    Timer crd_total_search_bucket_timer;
    crd_total_search_bucket_timer.start();
    Covering_Delta crdt(total_data_file, index, start, end, delta, 0);
    crd_total_search_bucket_timer.stop();
    //cout << "Covering Removing Duration Total Edge Stream Loading Buckets:\t" << crd_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer crd_total_edge_stream_timer;
    crd_total_edge_stream_timer.start();
    int xcrd = crdt.removed_tes();
    crd_total_edge_stream_timer.stop();
    //cout << "Covering Removing Duration Total Edge Stream Retrieval: \t" << crd_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Covering Removing Duration Results for Total Order: \t" << xcrd << endl;
    cout << "CRD\t" << crd_retrieve_edge_timer.elapsed_time() << "\t" << crd_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    Timer ead_search_bucket_timer;
    ead_search_bucket_timer.start();
    Existing_Delta ead(data_file, index, start, end, delta, 1);
    ead_search_bucket_timer.stop();
    //cout << "Existing Adding Duration Loading Buckets:\t" << ead_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer ead_retrieve_edge_timer;
    ead_retrieve_edge_timer.start();
    ead.get_added_duration_pairs();
    ead_retrieve_edge_timer.stop();
    //cout << "Existing Adding Duration Identifying Edges: \t" << ead_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer ead_retrieve_result_timer;
    ead_retrieve_result_timer.start();
    ead.retrieve_added_results();
    ead_retrieve_result_timer.stop();
    //cout << "Existing Adding Duration Retrieve Output: \t" << ead_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Existing Adding Duration Candidates:\t" << ead.index_max - ead.index_min << endl;
    cout << "# Existing Adding Duration Results:\t" << ead.number_added_results << endl;

    Timer ead_total_search_bucket_timer;
    ead_total_search_bucket_timer.start();
    Existing_Delta eadt(total_data_file, index, start, end, delta, 1);
    ead_total_search_bucket_timer.stop();
    //cout << "Existing Adding Duration Total Edge Stream Loading Buckets:\t" << ead_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer ead_total_edge_stream_timer;
    ead_total_edge_stream_timer.start();
    int xead = eadt.added_tes();
    ead_total_edge_stream_timer.stop();
    //cout << "Existing Adding Duration Total Edge Stream Retrieval: \t" << ead_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Existing Adding Duration Results for Total Order: \t" << xead << endl;
    cout << "EAD\t" << ead_retrieve_edge_timer.elapsed_time() << "\t" << ead_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    Timer erd_search_bucket_timer;
    erd_search_bucket_timer.start();
    Existing_Delta erd(data_file, index, start, end, delta, 0);
    erd_search_bucket_timer.stop();
    //cout << "Existing Removing Duration Loading Buckets:\t" << erd_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer erd_retrieve_edge_timer;
    erd_retrieve_edge_timer.start();
    erd.get_removed_duration_pairs();
    erd_retrieve_edge_timer.stop();
    //cout << "Existing Removing Duration Identifying Edges: \t" << erd_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer erd_retrieve_result_timer;
    erd_retrieve_result_timer.start();
    erd.retrieve_removed_results();
    erd_retrieve_result_timer.stop();
    //cout << "Existing Removing Duration Retrieve Output: \t" << erd_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Existing Removing Duration Candidates:\t" << erd.index_max - erd.index_min << endl;
    cout << "# Existing Removing Duration Results:\t" << erd.number_removed_results << endl;

    Timer erd_total_search_bucket_timer;
    erd_total_search_bucket_timer.start();
    Existing_Delta erdt(total_data_file, index, start, end, delta, 0);
    erd_total_search_bucket_timer.stop();
    //cout << "Existing Removing Duration Total Edge Stream Loading Buckets:\t" << erd_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer erd_total_edge_stream_timer;
    erd_total_edge_stream_timer.start();
    int xerd = erdt.removed_tes();
    erd_total_edge_stream_timer.stop();
    //cout << "Existing Removing Duration Total Edge Stream Retrieval: \t" << erd_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Existing Removing Duration Results for Total Order: \t" << xerd << endl;
    cout << "ERD\t" << erd_retrieve_edge_timer.elapsed_time() << "\t" << erd_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    Timer pad_search_bucket_timer;
    pad_search_bucket_timer.start();
    Persisting_Delta pad(data_file, index, start, end, delta, 1);
    pad_search_bucket_timer.stop();
    //cout << "Persisting Adding Duration Loading Buckets:\t" << pad_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer pad_retrieve_edge_timer;
    pad_retrieve_edge_timer.start();
    pad.get_added_duration_pairs();
    pad_retrieve_edge_timer.stop();
    //cout << "Persisting Adding Duration Identifying Edges: \t" << pad_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer pad_retrieve_result_timer;
    pad_retrieve_result_timer.start();
    pad.retrieve_added_results();
    pad_retrieve_result_timer.stop();
    //cout << "Persisting Adding Duration Retrieve Output: \t" << pad_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Persisting Adding Duration Candidates:\t" << pad.index_max - pad.index_min << endl;
    cout << "# Persisting Adding Duration Results:\t" << pad.number_added_results << endl;

    Timer pad_total_search_bucket_timer;
    pad_total_search_bucket_timer.start();
    Persisting_Delta padt(total_data_file, index, start, end, delta, 1);
    pad_total_search_bucket_timer.stop();
    //cout << "Persisting Adding Duration Total Edge Stream Loading Buckets:\t" << pad_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer pad_total_edge_stream_timer;
    pad_total_edge_stream_timer.start();
    int xpad = padt.added_tes();
    pad_total_edge_stream_timer.stop();
    //cout << "Persisting Adding Duration Total Edge Stream Retrieval: \t" << pad_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Persisting Adding Duration Results for Total Order: \t" << xpad << endl;
    cout << "PAD\t" << pad_retrieve_edge_timer.elapsed_time() << "\t" << pad_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    Timer prd_search_bucket_timer;
    prd_search_bucket_timer.start();
    Persisting_Delta prd(data_file, index, start, end, delta, 0);
    prd_search_bucket_timer.stop();
    //cout << "Persisting Removing Duration Loading Buckets:\t" << prd_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer prd_retrieve_edge_timer;
    prd_retrieve_edge_timer.start();
    prd.get_removed_duration_pairs();
    prd_retrieve_edge_timer.stop();
    //cout << "Persisting Removing Duration Identifying Edges: \t" << prd_retrieve_edge_timer.elapsed_time() << " \t seconds" << endl;

    Timer prd_retrieve_result_timer;
    prd_retrieve_result_timer.start();
    prd.retrieve_removed_results();
    prd_retrieve_result_timer.stop();
    //cout << "Persisting Removing Duration Retrieve Output: \t" << prd_retrieve_result_timer.elapsed_time() << " \t seconds" << endl;

    //cout << "# Persisting Removing Duration Candidates:\t" << prd.index_max - prd.index_min << endl;
    cout << "# Persisting Removing Duration Results:\t" << prd.number_removed_results << endl;

    Timer prd_total_search_bucket_timer;
    prd_total_search_bucket_timer.start();
    Persisting_Delta prdt(total_data_file, index, start, end, delta, 0);
    prd_total_search_bucket_timer.stop();
    //cout << "Persisting Removing Duration Total Edge Stream Loading Buckets:\t" << prd_total_search_bucket_timer.elapsed_time() << "  \t seconds" << endl;

    Timer prd_total_edge_stream_timer;
    prd_total_edge_stream_timer.start();
    int xprd = prdt.removed_tes();
    prd_total_edge_stream_timer.stop();
    //cout << "Persisting Removing Duration Total Edge Stream Retrieval: \t" << prd_total_edge_stream_timer.elapsed_time() << " \t seconds" << endl;
    //cout << "# Persisting Removing Duration Results for Total Order: \t" << xprd << endl;
    cout << "PRD\t" << prd_retrieve_edge_timer.elapsed_time() << "\t" << prd_total_edge_stream_timer.elapsed_time() << endl;
    cout << "*********************************************************" << endl;

    return 0;
}
