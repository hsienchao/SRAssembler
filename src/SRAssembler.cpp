/*
 * SRAssembler.cpp
 *
 *  Created on: Oct 12, 2011
 *      Author: Hsien-chao Chou
 */

#include <ctime>
#include "SRAssembler.h"
#include "SRAssemblerMaster.h"
#include "SRAssemblerSlave.h"
#include "SRAssemblerSlave.h"
#include <vector>


SRAssembler* SRAssembler::_srassembler = NULL;

SRAssembler::SRAssembler() {
	// TODO Auto-generated constructor stub

}

SRAssembler::~SRAssembler() {
	// TODO Auto-generated destructor stub
}

int SRAssembler::init(int argc, char * argv[], int rank, int size) {
    //set the default values
	init_match_length = INIT_MATCH_LENGTH_PROTEIN;
	recur_match_length = RECUR_MATCH_LENGTH;
	mismatch_allowed = MISMATCH_ALLOWED_PROTEIN;
	num_rounds = NUM_ROUNDS;
	min_contig_size = MIN_CONTIG_SIZE;
	max_contig_size = MAX_CONTIG_SIZE;
	verbose = VERBOSE;
	assembly_round = ASSEMBLY_ROUND;
	reads_per_file = READS_PER_FILE;
	fastq_format = FASTQ_INTERLEAVED;
	out_dir = OUT_DIR;
	data_dir = "";
	type = QUERY_TYPE;
	assembler_program = ASSEMBLER_PROGRAM;
	gene_finding_program = GENE_FINDING_PROGRAM;
	spliced_alignment_program = SPLICED_ALIGNMENT_PROGRAM;
	start_k = START_K;
	end_k = END_K;
	step_k = STEP_K;
	clean_round = CLEAN_ROUND;
	over_write = OVER_WRITE;
	check_gene_assembled = CHECK_GENE_ASSEMBLED;
	preprocessing_only = PREPROCESSING_ONLY;
	min_score = MIN_SCORE;
	min_coverage = MIN_COVERAGE;
	bool k_format = true;
	this->rank = rank;
	this->size = size;
	unsigned int insert_size = INSERT_SIZE;
	string left_read = "";
	string right_read = "";
	//parse command line options
	bool default_e = true;
	bool default_i = true;
	usage = "usage:\n";
	usage.append("\n");
	usage.append("    SRAssembler [options] -q query_file -P parameter_file -s species\n");
	usage.append("                [-f library_file or -1 reads_file1 -2 reads_file2]\n");
	usage.append("\n");
	usage.append("-q: Required. The FASTA format query file.\n");
	usage.append("-s: Required. Species name. Options:'human', 'mouse', 'rat', 'chicken',\n");
	usage.append("    'drosophila', 'nematode', 'fission_yeast', 'aspergillus', 'arabidopsis',\n");
	usage.append("    'maize', 'rice', 'medicago'.\n");
	usage.append("-P: Required. Parameter configuration file\n");
	usage.append("-f: Required if you do not specify -1 option. Library file\n");
	usage.append("-1: Required if you do not specify library file. The option can be used to\n");
	usage.append("    specify the single-end file name or the left end file name for paired-end\n");
	usage.append("    reads.\n");
	usage.append("-2: Right end file name for paired-end reads\n");
	usage.append("-t: Query file type: Options: 'protein', 'cdna' [Default: " + QUERY_TYPE + "]\n");
	usage.append("-o: Output directory [Default: current directory]\n");
	usage.append("-R: Preprocessing directory [Default: output directory/" + READS_DATA + "]\n");
	usage.append("-m: Minimum contig length to be reported [Default: " + int2str(MIN_CONTIG_SIZE) + "]\n");
	usage.append("-M: Maximum contig length to be reported [Default: " + int2str(MAX_CONTIG_SIZE) + "]\n");
	usage.append("-e: Minimum score [Default: " + double2str(MIN_SCORE) + "]\n");
	usage.append("-c: Minimum coverage [Default: " + double2str(MIN_COVERAGE) + "]\n");
	usage.append("-k: The k-mer set for the internal assembler. The format is:\n");
	usage.append("    start_k:interval:end_k. The start_k and end_k must be odd value, and The\n");
	usage.append("    interval must be even value. For example, '15:10:45' means k-mer value\n");
	usage.append("    15, 25, 35, 45 will be tested.[Default: " + int2str(START_K) + ":" + int2str(STEP_K) + ":" + int2str(END_K) + "]\n");
	usage.append("-z: Insert size of the paired-end reads [Default: " + int2str(INSERT_SIZE) + "]\n");
	usage.append("-n: Number of rounds [Default: " + int2str(NUM_ROUNDS) + "]\n");
	usage.append("-x: Number of reads of split file [Default: " + int2str(READS_PER_FILE) + "]\n");
	usage.append("-A: The internal assembler. Options: 0=>SOAPdenovo 1=>ABySS [Default: " + int2str(ASSEMBLER_PROGRAM) + "]\n");
	usage.append("-S: Spliced alignment program. Options:0=>GenomeThreader, 1=>GeneSeqer,\n");
	usage.append("    2=>Exonerate [Default: " + int2str(SPLICED_ALIGNMENT_PROGRAM) + "]\n");
	usage.append("-G: Gene finding program. Options:0=>None, 1=>Snap [Default: " + int2str(GENE_FINDING_PROGRAM) + "]\n");
	usage.append("-p: Do preprocessing only\n");
	usage.append("-i: Do not start from the last checkpoint\n");
	usage.append("-l: Do not check if the genes are assembled in each round\n");
	usage.append("-a: The round number the assembling starts [Default: " + int2str(ASSEMBLY_ROUND) + "]\n");
	usage.append("-r: The round number to remove the unrelated contigs and reads. For example,\n");
	usage.append("    “3” means SRAssembler do the cleaning at round 3, 6, 9, 12... [Default: " + int2str(CLEAN_ROUND) + "]\n");
	usage.append("-v: Verbose output\n");
	usage.append("-h: Show the usage");


	char c;
	while((c = getopt(argc, argv, "q:s:t:1:2:o:z:n:m:x:r:e:c:a:k:c:f:M:S:P:A:G:R:vpilh")) != -1) {
		 switch (c){
			 case 'q':
				 query_file = optarg;
				 break;
			 case 's':
				 species = optarg;
				 break;
			 case 't':
				 type = optarg;
				 break;
			 case '1':
				 left_read = optarg;
				 break;
			 case '2':
				 right_read = optarg;
				 break;
			 case 'o':
				 out_dir = optarg;
				 break;
			 case 'R':
			 	 data_dir = optarg;
			 	 break;
			 case 'f':
				 library_file = optarg;
				 break;
			 case 'k': {
				 vector<string> tokens;
				 tokenize(optarg, tokens, ":");
			 	 if (tokens.size() != 3)
			 		k_format = false;
			 	 start_k = str2int(tokens[0]);
			 	 step_k = str2int(tokens[1]);
			 	 end_k = str2int(tokens[2]);
			 	 if (start_k <= 0 || start_k > 150 || start_k % 2 == 0) {
			 		k_format = false;
			 		break;
			 	 }
			 	 if (end_k <= 0 || end_k > 150 || end_k % 2 == 0 || end_k <= start_k) {
			 		k_format = false;
			 		break;
			 	 }
			 	 if (step_k <= 0 || step_k > 150 || step_k % 2 == 1) {
			 		k_format = false;
			 		break;
			 	 }
			 	 int k_value = start_k;
			 	 while (k_value < end_k)
			 		k_value += step_k;
			 	 if (k_value > step_k)
			 		 end_k = k_value;
			 	 break;
			 }
			 case 'z':
				 insert_size = str2int(optarg);
				 break;
			 case 'A':
			 	 assembler_program = str2int(optarg);
			 	 break;
			 case 'S':
				 spliced_alignment_program = str2int(optarg);
				 break;
			 case 'G':
			 	 gene_finding_program = str2int(optarg);
			 	 break;
			 case 'P':
			 	 param_file = optarg;
			 	 break;
			 case 'n':
				 num_rounds = str2int(optarg);
				 break;
			 case 'm':
				 min_contig_size = str2int(optarg);
				 break;
			 case 'e':
				 min_score = str2double(optarg);
				 break;
			 case 'c':
				 min_coverage = str2double(optarg);
				 break;
			 case 'M':
				 max_contig_size = str2int(optarg);
				 break;
			 case 'r':
				 clean_round = str2int(optarg);
				 break;
			 case 'x':
				 reads_per_file = str2int(optarg);
				 break;
			 case 'a':
				 assembly_round = str2int(optarg);
				 if (assembly_round == 0)
					 assembly_round = 9999;
				 break;
			 case 'v':
				 verbose = 1;
				 break;
			 case 'p':
			 	 preprocessing_only = 1;
			 	 break;
			 case 'i':
			 	 over_write = 1;
			 	 break;
			 case 'l':
				 check_gene_assembled = 0;
				 break;
			 case 'h':
				 show_usage();
				 return -1;
			 	 break;
			 case '?':
				 string msg = "input error! unknown option : -";
				 msg.append(1,optopt);
				 print_message(msg);         //unknown options
				 return -1;
		  }
	}
    if (argc == 1){
    	show_usage();
    	return -1;
    }
	tmp_dir = out_dir + "/tmp";
	if (data_dir == "")
	    data_dir = out_dir + "/" + READS_DATA;
	results_dir = out_dir + "/output";
	intermediate_dir = results_dir + "/intermediates";
	log_file = results_dir + "/msg.log";
	spliced_alignment_output_file = results_dir + "/output.aln";
	gene_finding_output_file = results_dir + "/output.ano";
	//spliced_alignment_gsq_file = results_dir + "/output.gsq";
	final_contig_file = results_dir + "/all_contigs.fasta";
	summary_file = results_dir + "/summary.html";
	aligned_contig_file = results_dir + "/hit_contigs.fasta";

	int level = Logger::LEVEL_INFO;
	if (verbose)
		level = Logger::LEVEL_DEBUG;
	logger = Logger::getInstance(level, log_file);


	if (param_file != ""){
		if (!file_exists(param_file)){
			print_message("Parameter file : " + param_file + " does not exist!");
			return -1;
		}
	}

	if (library_file != ""){
		if (!file_exists(library_file)){
			print_message("library file: " + library_file + " does not exist!");
			return -1;
		}
		if (!read_library_file())
			return -1;
	} else {
		if (left_read == ""){
			print_message("-1 or -r or library file is required");
			return -1;
		}
		Library lib(0, this->data_dir, this->tmp_dir,this->logger);
		lib.set_format(FORMAT_FASTQ);
		lib.set_left_read(left_read);
		if (right_read != ""){
			lib.set_paired_end(true);
			lib.set_right_read(right_read);
			lib.set_insert_size(insert_size);
			if (!file_exists(right_read)){
				print_message("file: " + right_read + " does not exist!");
				return -1;
			}
		}
		this->libraries.push_back(lib);
	}

	//check missing options
	if (num_rounds <= 0){
		print_message("-n must be larger than 0");
		return -1;
	}
	if (reads_per_file <= 1000){
		print_message("-x value is not valid or too small");
		return -1;
	}

	if (type != TYPE_PROTEIN && type != TYPE_CDNA){
		print_message("-t must be 'protein' or 'cdna'");
		return -1;
	} else {
		if (default_e && type == TYPE_CDNA)
			mismatch_allowed = MISMATCH_ALLOWED_CDNA;
		if (default_i && type == TYPE_CDNA)
			init_match_length = INIT_MATCH_LENGTH_CDNA;
	}
	if (species != "human" && species != "mouse" && species != "rat" && species != "chicken" && species != "drosophila" && species != "nematode" && species != "fission_yeast" && species != "aspergillus" && species != "arabidopsis" && species != "maize" && species != "rice" && species != "medicago"){
		print_message("-s is required");
		return -1;
	}
	if (!k_format){
		print_message("-k format error. The format is : start_k:interval:end_k. The start_k and end_k must be odd value, and The interval must be even value.");
		return -1;
	}
	//check the existence of read files
	if (!file_exists(query_file)){
		print_message("file: " + query_file + " does not exist!");
		return -1;
	}

	if (!file_exists(out_dir)){
		print_message("output directory: " + out_dir + " does not exist!");
		return -1;
	}

	return 0;
}

Params SRAssembler::read_param_file(string program_name) {
	ifstream param_file(this->param_file.c_str());
	string line;
	Params params;
	bool found_program = false;
	while (getline(param_file, line)){
		line = trim(line);
		if (line.length() == 0) continue;
		if (line == ("[" + program_name + "]")) {
			found_program = true;
			continue;
		}
		if (line.substr(0,1) == "#") continue;
		if (found_program && line.substr(0,1) == "[") break;
		if (found_program){
			vector<string> tokens;
			tokenize(line, tokens, "=");
			if (tokens.size() == 2){
				string param = trim(tokens[0]);
				string value = trim(tokens[1]);
				params.insert(Params::value_type(param, value));
			}
		}
	}
	return params;
}

bool SRAssembler::read_library_file() {
	ifstream lib_file(this->library_file.c_str());
	string line;
	Library* lib = NULL;
	while (getline(lib_file, line)){
		line = trim(line);
		if (line == "[LIBRARY]") {
			if (lib != NULL){
				if (lib->get_left_read() == ""){
					print_message("r1 file is expected in config file!");
					return false;
				}
				this->libraries.push_back(*lib);
			}
			lib = new Library(this->libraries.size(), this->data_dir, this->tmp_dir, this->logger);
		} else {
			vector<string> tokens;
			tokenize(line, tokens, "=");
			if (tokens.size() == 2){
				string param = trim(tokens[0]);
				string value = trim(tokens[1]);
				if (param == "r1" && lib != NULL) {
					lib->set_left_read(value);
					if (!file_exists(lib->get_left_read())) {
						print_message("r1 file in config file: " + lib->get_left_read() + " does not exist!");
						return false;
					}
				}
				if (param == "r2" && lib != NULL) {
					lib->set_right_read(value);
					if (!file_exists(lib->get_right_read())) {
						print_message("r2 file in config file: " + lib->get_right_read() + " does not exist!");
						return false;
					}
					lib->set_paired_end(true);
				}

				if (param == "insert_size" && lib != NULL) {
					lib->set_insert_size(str2int(value));
				}
				if (param == "direction" && lib != NULL) {
					lib->set_reversed(str2int(value));
				}
				if (param == "format" && lib != NULL) {
					if (value == "fastq") {
						lib->set_format(FORMAT_FASTQ);
					}
					else if (value == "fasta") {
						lib->set_format(FORMAT_FASTA);
					} else {
						print_message("format in config file should be 'fastq' or 'fasta'!");
						return false;
					}
				}
			}
		}
	}
	if (lib != NULL){
		if (lib->get_left_read() == ""){
			print_message("r1 file is expected in config file!");
			return false;
		}
		this->libraries.push_back(*lib);
	}
	if (this->libraries.size() == 0){
		print_message(" No [LIBRARY] section found in config file!");
		return false;
	}
	return true;

}

int SRAssembler::get_file_count(string search_pattern){
	string cmd = "ls -l " + search_pattern + " | wc -l";
	logger->debug(cmd);
	return str2int(run_shell_command_with_return(cmd));
}

void SRAssembler::do_preprocessing(int lib_idx, int file_part){
	Library lib = this->libraries[lib_idx];

	logger->info("preprocessing lib " + int2str(lib_idx + 1) + ", reads file (" + int2str(file_part) + "/" + int2str(lib.get_num_parts()) + ")");
	char suffixc[3];
	suffixc[0] = (char)(((file_part-1) / 26) + 97);
	suffixc[1] = (char)(((file_part-1) % 26) + 97);
	suffixc[2] = '\0';
	string suffix(suffixc);
	string left_src_read = lib.get_prefix_split_src_file(lib.get_left_read()) + suffix;
	string right_src_read = "";
	if (lib.get_paired_end())
	    right_src_read = lib.get_prefix_split_src_file(lib.get_right_read()) + suffix;
	ifstream left_file(left_src_read.c_str());
	ifstream right_file;
	if (lib.get_paired_end()){
		right_file.open(right_src_read.c_str(), ios_base::in);
	}
	string left_header = "";
	string right_header = "";
	string left_seq = "";
	string right_seq = "";
	string left_qual = "";
	string right_qual = "";
	string plus;
	ofstream split_read_fasta_file;
	ofstream split_read_fastq_file;
	split_read_fasta_file.open(lib.get_split_file_name(file_part, FORMAT_FASTA).c_str(), ios_base::out);
	if (lib.get_format() == FORMAT_FASTQ)
	    split_read_fastq_file.open(lib.get_split_file_name(file_part, FORMAT_FASTQ).c_str(), ios_base::out);
	while (getline(left_file, left_header)) {
		// get read data point, which includes 4 lines
		string lead_chr = (lib.get_format() == FORMAT_FASTQ)? "@" : ">";
		if (left_header.substr(0,1) == lead_chr){
		 //save left-end reads
			getline(left_file, left_seq);
			if (lib.get_format() == FORMAT_FASTQ) {
				getline(left_file, plus);
				getline(left_file, left_qual);
			}
			if (lib.get_paired_end()){
				 //save right-end reads
				 while (getline(right_file, right_header))
					 if (right_header.substr(0,1) == lead_chr)
						 break;
				 getline(right_file, right_seq);
				 if (lib.get_format() == FORMAT_FASTQ) {
					 getline(right_file, plus);
					 getline(right_file, right_qual);
				 }
			}
			if (lib.get_format() == FORMAT_FASTQ) {
				split_read_fastq_file << left_header << endl
									  << left_seq << endl
									  << "+" << endl
									  << left_qual << endl;
			}
			split_read_fasta_file << ">" << left_header.substr(1) << endl << left_seq << endl;
			if (lib.get_paired_end()){
				if (lib.get_format() == FORMAT_FASTQ) {
					split_read_fastq_file << right_header << endl
								 << right_seq << endl
								 << "+" << endl
								 << right_qual << endl;
				}
				split_read_fasta_file << ">" << right_header.substr(1) << endl << right_seq << endl;
			}
		 }
	}
	split_read_fasta_file.close();
	if (lib.get_format() == FORMAT_FASTQ)
		split_read_fastq_file.close();
	left_file.close();
	if (lib.get_paired_end())
		right_file.close();
	string cmd = "rm " + left_src_read + " " + right_src_read;
	run_shell_command(cmd);
}
void SRAssembler::send_code(const int& to, const int& action, const int& value1, const int& value2, const int& value3){
	mpi_code code;
	code.action = action;
	code.value1 = value1;
	code.value2 = value2;
	code.value3 = value3;
	mpi_send(get_mpi_code_value(code), to);
}
void SRAssembler::broadcast_code(const int& action, const int& value1, const int& value2, const int& value3){
	mpi_code code;
	code.action = action;
	code.value1 = value1;
	code.value2 = value2;
	code.value3 = value3;
	mpi_bcast(get_mpi_code_value(code));
}
string SRAssembler:: get_index_name(int round){
   return tmp_dir + "/round" + int2str(round);
}
string SRAssembler:: get_index_fasta_file_name(int round){
	if (round > 1){
		if (assembly_round < round)
		    return get_contig_file_name(round-1);
		else {
			string joined_file = tmp_dir + "/output_matched_joined.fasta";
			string cmd;
			for (unsigned i=0;i<this->libraries.size();i++){
				string left_file = tmp_dir + "/output_matched_left_" + int2str(i) + ".fasta";
				string right_file = tmp_dir + "/output_matched_right_" + int2str(i) + ".fasta";
				if (libraries[i].get_file_extension() == "fastq") {
					fastq2fasta(tmp_dir + "/output_matched_left_" + int2str(i) + ".fastq", left_file);
					if (libraries[i].get_paired_end())
						fastq2fasta(tmp_dir + "/output_matched_right_" + int2str(i) + ".fastq", right_file);
				}
				if (libraries[i].get_paired_end()){
					cmd = "cat " + left_file + " " + right_file + " >> " + joined_file;
					logger->debug(cmd);
					run_shell_command(cmd);
				}else
					cmd = "cat " + left_file + " >> " + joined_file;
					logger->debug(cmd);
					run_shell_command(cmd);
			}
			return joined_file;
		}
	}
	return query_file;
}
string SRAssembler:: get_contig_file_name(int round){
	return intermediate_dir + "/contigs-" + int2str(round) + ".fasta";
}

string SRAssembler:: get_mapped_reads_file_name(int round){
	return tmp_dir + "/matched_reads_" + int2str(round) + "_" + int2str(this->rank) + ".list";;
}

int SRAssembler::do_alignment(int round, int lib_idx, int idx) {
	Library lib = this->libraries[lib_idx];
	logger->info("Aligning: round = " + int2str(round) + " Lib (" + int2str(lib_idx+1) + "/" + int2str(this->libraries.size()) + "), Reads (" + int2str(idx) + "/" + int2str(lib.get_num_parts()) + ")");
	Aligner* aligner = get_aligner(round);
	string program_name = aligner->get_program_name();
	if (round == 1)
		program_name += "_init";
	Params params = this->read_param_file(program_name);
	aligner->do_alignment(get_index_name(round), get_type(round), get_match_length(round), get_mismatch_allowed(round), lib.get_split_file_name(idx, aligner->get_format()), params, get_output_file_name(round, lib_idx, idx));
	int ret = aligner->parse_output(get_output_file_name(round, lib_idx, idx), mapped_reads, lib.get_split_file_name(idx, lib.get_format()), lib.get_matched_left_read_name(round, idx), lib.get_matched_right_read_name(round, idx), fastq_format,  lib.get_format());
	string cmd = "rm -f " +  get_output_file_name(round, lib_idx, idx);
	save_mapped_reads(round);
	run_shell_command(cmd);
	return ret;
}

void SRAssembler::do_assembly(int round, int k) {
	logger->info("Doing assembly: round = " + int2str(round) + ", k = " + int2str(k));
	Assembler* assembler = get_assembler();
	assembler->do_assembly(k, this->libraries, tmp_dir + "/assembled_" + int2str(k) + "_" + int2str(round));
}

int SRAssembler::do_assembly(int round) {
	unsigned int best_k = 0;
	unsigned int max_longest_contig = 0;
	for (unsigned int k=15;k<=25;k+=5) {
		do_assembly(round, k);
		Assemble_stat stat = get_assemble_stat(round, k);

		logger->info("longest contig is: " + int2str(stat.longest_contig) + " with k = " + int2str(k));
		if (stat.longest_contig == 0) logger->debug("contig length 0, wrong assembler setting?");
		if (stat.longest_contig > max_longest_contig){
			best_k = k;
			max_longest_contig = stat.longest_contig;
		}
	}
	logger->info("The best k is " + int2str(best_k));
	//prepare_contig_file(round, best_k);
	return max_longest_contig;
}

void SRAssembler::do_spliced_alignment() {
	logger->info("Doing the final spliced alignment...");
	SplicedAligner* spliced_aligner = get_spliced_aligner();
	string program_name = spliced_aligner->get_program_name();
	Params params = this->read_param_file(program_name);
	spliced_aligner->do_spliced_alignment(this->final_contig_file, type, this->query_file, this->species, params, this->spliced_alignment_output_file,this->aligned_contig_file);
	spliced_aligner->get_aligned_contigs(min_score, min_coverage, this->final_contig_file, this->aligned_contig_file, this->spliced_alignment_output_file);
	logger->info("Done.");
}

string_map SRAssembler::do_spliced_alignment(int round) {
	logger->info("Doing spliced alignment...");
	SplicedAligner* spliced_aligner = get_spliced_aligner();
	string program_name = spliced_aligner->get_program_name();
	Params params = this->read_param_file(program_name);
	string contig_file = get_contig_file_name(round);
	string output_file = tmp_dir + "/alignment_" + int2str(round) + ".aln";
	string hit_file = tmp_dir + "/hit_alignment_" + int2str(round) + ".aln";
	spliced_aligner->do_spliced_alignment(contig_file, type, this->query_file, this->species, params, output_file,hit_file);;
	string_map query_map = spliced_aligner->get_aligned_contigs(min_score, min_coverage, contig_file, hit_file, output_file);
	spliced_aligner->clean_files(contig_file);
	logger->info("Done.");
	return query_map;
}

void SRAssembler::do_gene_finding() {
	GeneFinder* gene_finder = get_gene_finder();
	if (gene_finder == NULL)
		return;
	if (!gene_finder->is_available())
		return;
	logger->info("Doing gene finding...");
	string program_name = gene_finder->get_program_name();
	Params params = this->read_param_file(program_name);
	gene_finder->do_gene_finding(this->aligned_contig_file, this->species, params, this->gene_finding_output_file);
	logger->info("Done.");
}

string SRAssembler::get_assembled_file_name(int round, int k){
	return get_assembler()->get_output_contig_file_name(tmp_dir + "/assembled_" + int2str(k) + "_" + int2str(round));
}
string SRAssembler::get_assembled_scaf_file_name(int round, int k){
	return get_assembler()->get_output_scaffold_file_name(tmp_dir + "/assembled_" + int2str(k) + "_" + int2str(round));
}
Aligner* SRAssembler::get_aligner(int round) {
	int aligner_type = Aligner::DNA_ALIGNER;
	if (round == 1 && type == "protein")
		aligner_type = Aligner::PROTEIN_ALIGNER;
	Aligner* aligner = Aligner::getInstance(aligner_type, logger->get_log_level(), logger->get_log_file());
	return aligner;
}

Assembler* SRAssembler::get_assembler(){
	return Assembler::getInstance(this->assembler_program, logger->get_log_level(), logger->get_log_file());
}

SplicedAligner* SRAssembler::get_spliced_aligner(){
	return SplicedAligner::getInstance(this->spliced_alignment_program, logger->get_log_level(), logger->get_log_file());
}

GeneFinder* SRAssembler::get_gene_finder(){
	return GeneFinder::getInstance(this->gene_finding_program, logger->get_log_level(), logger->get_log_file());
}

Logger* SRAssembler::get_logger(){
	return logger;
}

void SRAssembler::create_index(int round) {
	Aligner* aligner = get_aligner(round);
	aligner->create_index(get_index_name(round), get_type(round), get_index_fasta_file_name(round));
}

string SRAssembler:: get_type(int round){
   return (round == 1)? type: TYPE_CDNA;
}

int SRAssembler::get_match_length(int round){
	return (round == 1)? init_match_length: recur_match_length;
}

int SRAssembler::get_mismatch_allowed(int round) {
	return (round == 1)? mismatch_allowed: 0;
}

string SRAssembler::get_output_file_name(int round, int lib_idx, int idx){
    return tmp_dir + "/output_" + int2str(round) + "_" + int2str(lib_idx+1) + "_" + int2str(idx) +".aln";
}

void SRAssembler::merge_mapped_files(int round){
	for (unsigned int lib_idx=0;lib_idx<this->libraries.size();lib_idx++){
		Library lib = this->libraries[lib_idx];
		logger->debug("Merge files...");
		//copy reads we have so far, for debugging
		string left_files = tmp_dir + "/output_matched_left_" + int2str(round) + "_" + int2str(lib_idx + 1) + "_*";
		string cmd = "cat " + left_files + " >> " + lib.get_matched_left_read_name();
		logger->debug(cmd);
		run_shell_command(cmd);
		cmd = "rm -f " + left_files;
		logger->debug(cmd);
		run_shell_command(cmd);
		if (lib.get_paired_end()) {
			string right_files = tmp_dir + "/output_matched_right_" + int2str(round) + "_" + int2str(lib_idx + 1) + "_*";
			cmd = "cat " + right_files + " >> " + lib.get_matched_right_read_name();
			logger->debug(cmd);
			run_shell_command(cmd);
			cmd = "rm -f " + right_files;
			logger->debug(cmd);
			run_shell_command(cmd);
		}
		//cmd = "cat " + tmp_dir + "/output_matched_joined_" + int2str(round) + "_" + int2str(lib_idx) + "* > " + tmp_dir + "/output_matched_joined_" + int2str(round) + "_" + int2str(lib_idx) + ".fasta";
		if (round > 1)
			run_shell_command("cp " + lib.get_matched_left_read_name() + " " + lib.get_matched_left_read_name(round));
		if (lib.get_paired_end() && round > 1)
			run_shell_command("cp " + lib.get_matched_right_read_name() + " " + lib.get_matched_right_read_name(round));
	}
	string cmd = "rm -f " + get_index_name(round) + ".*";;
	logger->debug(cmd);
	run_shell_command(cmd);
	logger->debug("done.");
}

int SRAssembler::get_reads_count(int round){
	int count = 0;
	for (unsigned short int lib_idx=0; lib_idx< this->libraries.size();lib_idx++) {
		count += get_read_count(this->libraries[lib_idx].get_matched_left_read_name(), this->libraries[lib_idx].get_format());
	}

	return count;
}
Assemble_stat SRAssembler::get_assemble_stat(int round, int k){
	logger->debug("Counting longest contig length:" + get_assembled_file_name(round, k));
	ifstream contig_file(get_assembled_file_name(round, k).c_str());
	string line;
	vector<int> lens;
	Assemble_stat stat;
	stat.longest_contig = 0;
	stat.total_contig = 0;
	stat.n50 = 0;
	stat.n90 = 0;
	int total_length = 0;
	unsigned int contig_length = 0;
	while (getline(contig_file, line)){
		if (line.substr(0,1) == ">"){
			if (contig_length > min_contig_size){
				lens.push_back(contig_length);
				total_length += contig_length;
				stat.total_contig++;
				if (stat.longest_contig < contig_length)
					stat.longest_contig = contig_length;
			}
			contig_length = 0;
		} else
			contig_length += line.length();
	}
	if (contig_length > min_contig_size){
		total_length += contig_length;
		stat.total_contig++;
		lens.push_back(contig_length);
		if (stat.longest_contig < contig_length)
			stat.longest_contig = contig_length;
	}
	contig_file.close();
	sort(lens.begin(), lens.end());
	int cumu_len = 0;
	for (short i=lens.size()-1;i>=0;i--){
		cumu_len += lens[i];
		if (cumu_len >= total_length/2.0 && stat.n50 == 0)
			stat.n50 = lens[i];
		if (cumu_len >= (double)total_length*0.9){
			stat.n90 = lens[i];
			break;
		}
	}
	return stat;
}


void SRAssembler::save_mapped_reads(int round){
	string mapped_file = get_mapped_reads_file_name(round);
	ofstream mapped_file_stream(mapped_file.c_str());
	for (unordered_set<string>::iterator it = mapped_reads.begin();it != mapped_reads.end(); ++it)
	     mapped_file_stream << *it << endl;
	mapped_file_stream.close();
}

void SRAssembler::load_mapped_reads(int round){
	string mapped_file = get_mapped_reads_file_name(round);
	logger->info("Loading the mapped reads of round " + int2str(round) + " (" + int2str(rank) + "/" + int2str(size-1) + ")");
	ifstream mapped_file_stream(mapped_file.c_str());
	string seq_id;
	while (getline(mapped_file_stream, seq_id)){
		mapped_reads.insert(seq_id);
	}
	mapped_file_stream.close();
}

SRAssembler* SRAssembler::getInstance(int pid){
	if (pid == 0){
		_srassembler = new SRAssemblerMaster();
		return _srassembler;
	} else {
		_srassembler = new SRAssemblerSlave();
		return _srassembler;
	}
	return NULL;
}

void finalized(){
	mpi_finalize();
}

int main(int argc, char * argv[] ) {

	time_t now = time(0);
	int rank ,size;

	SRAssembler* srassembler = NULL;
	try {
		mpi_init(argc,argv);
		size=mpi_get_size();
		rank=mpi_get_rank();

		srassembler = SRAssembler::getInstance(rank);
		int ret = srassembler->init(argc, argv, rank, size);
		if (ret == -1) {
			throw -1;
		}
		srassembler->do_preprocessing();
		srassembler->do_walking();
	} catch (int e) {
		mpi_code code;
		code.action = ACTION_EXIT;
		code.value1 = 0;
		code.value2 = 0;
		mpi_bcast(get_mpi_code_value(code));
		finalized();
		return -1;
	}
	finalized();
	if (rank == 0) {
		string str = "Execution time: " + int2str(time(0) - now) + " seconds";
        srassembler->get_logger()->info(str);
	}
	return 0;
}
