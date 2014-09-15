SRAssembler
===========
Local assembly of genomic reads by homologous sequences

##### Contents
[Overview] (#overview)
[Installation] (#installation)
[Short Manual] (#manual)
[FAQs] (#faq)

<a name="overview"/>
###Overview

SRAssembler is a pipeline program that can assemble genomic DNA reads using homologous sequences as query input. We first align the reads that can locally be mapped onto those query sequences. These mapped reads then serve as seed reads, which are used to find other reads partially mapped to them. This in-silico ”chromosome walking” strategy can be used to recursively gather reads that are associated with regions of interest. The mined reads can be further assembled into contigs, which can subsequently and easily be annotated by current spliced alignment programs. This idea is based on the in-silico chromosome walking program called Tracembler, which assembles entries from the outdated NCBI Trace Archive.

In this pipeline, we use the following steps to assemble the homologous loci of DNA regions of interest:

Step1: Align protein or cDNA sequences to whole genome shotgun reads by Vmatch, then we get initial seed reads.

Step2: Start the "chromosome walking" by recursively finding reads mapped to seed reads or contigs. In each round, if option -a specified, all reads found at this point will be assembled as contigs, and these contigs will serve as new query sequences of next round.

Step3: The recursion is terminated when no more reads could be found or the maximum number of rounds is reached, which is specified in -n option.

Step4: Several assemblies with different k-mer values, the overlap length of reads, are generated by SOAPdenovo. The assembly with longest contig will be selected. You can specify the minimum contig length reported (option -m).

Step5: The gene structure is predicted by GenomeThreader, GeneSeqer, Exonerate or Snap.

<a name="installation"/>
### Installation
##### Prerequisite programs

  1. [Vmatch 2.0](http://www.vmatch.de/)
  
  2. [SOAPdenovo](http://soap.genomics.org.cn/soapdenovo.html) or [ABySS 1.3.4](http://www.bcgsc.ca/platform/bioinfo/software/abyss)
  
  3. [GenomeThreader 1.0.8](http://www.genomethreader.org/) or [GeneSeqer](http://brendelgroup.org/bioinformatics2go/GeneSeqer.php) or [Exonerate 2.2](http://www.ebi.ac.uk/~guy/exonerate/)

  4. [Bowtie first version(0.12.7 or newer)](http://bowtie-bio.sourceforge.net/index.shtml)

  5. [C++ Boost libraries](http://www.boost.org/users/download/)

  6. For parallel version, you need to install [openmpi](http://www.open-mpi.org/) or [mpich2](http://www.mcs.anl.gov/research/projects/mpich2/).
     
     For OpenMPI you may need to set LD_PRELOAD variable. For example, in bash:

     export LD_PRELOAD=/usr/lib64/openmpi/lib/libmpi_cxx.so

  7. Optional: [SNAP](http://korflab.ucdavis.edu/software.html)

  Make sure the execution files above are on your PATH environment variable.

##### Serial version installation

  1. Type 'cd src' to change the current directory to the source folder.
  
  2. Type 'make' to compile the package. The executable file 'SRAssembler' should be generated in 'bin' folder.

  3. Type `make install' to install the program to /usr/local/bin.

##### Parallel version installation

  1. Type 'cd src' to change the current directory to the source folder.
  
  2. Type 'make mpi' to compile the package. The executable file 'SRAssembler_MPI' should be generated in 'bin' folder.

  3. Type `make install' to install the program to /usr/local/bin.

##### Installation of both version

  1. Type 'cd src' to change the current directory to the source folder.

  2. Type 'make' to compile the package. The executable file 'SRAssembler' should be generated in 'bin' folder.

  3. Type 'make clean' to clear the object files.
  
  4. Type 'make mpi' to compile the package. The executable file 'SRAssembler_MPI' should be generated in 'bin' folder.

  5. Type `make install' to install the program to /usr/local/bin.

##### C++ Boost libraries option

In most linux systems, the C++ Boost libraries are already installed. If yours are not, please download it from [boost site](http://www.boost.org/users/download/). They do not need to be compiled. If these header files are not in /usr/include/boost, please use with-boost option to specify it. Like this:

    make with-boost=boost_path or 

    make mpi with-boost=boost_path

<a name="manual"/>
### Short Manual

The complete manual can be downloaded [here](https://github.com/hsienchao/SRAssembler/blob/master/doc/SRAssembler.pdf).

##### Inputs

   * Paired-end DNA reads in FASTQ format.
   * Query protein/cDNA sequence file of related species in FASTA format.

##### Using SRAssembler
   ```
   SRAssembler -q LOC_Os06g04560.pep -1 reads1_200.fq -2 reads2_200.fq -s arabidopsis -t protein -n 13 -P Params.conf -o testout1
   ```

   Inputs:
   -q: the query protein file
   -1: the left end reads file
   -2: the right end reads file
   -s: species
   -t: type (protein or cDNA)
   -n: the maximum round to stop the program
   -P: detailed paratmeter file (see manual)
   -o: output directory

##### Outputs
   * output_dir/output/summary.html: The summary html file.
   * output_dir/output/all_contigs.fasta: All assembled contigs.
   * output_dir/output/hit_contigs.fasta: The contigs identified by spliced alignment program.
   * output_dir/output/output.aln: The spliced alignment report.
   * output_dir/output/output.ano: The ab initio gene prediction report.
   * output_dir/output/msg.log: The detailed log file.
   * output_dir/output/intermediates directory: All intermediate contigs are saved
in this directory. For example, the contigs assembled in round 3 is contigs-3.fasta.
output_dir/reads_data directory : The original reads data files will be split into
smaller parts in FASTA format. We save these preprocessed files in this directory.
If you want to rerun this dataset, SRAssembler will read the preprocessed data
directly. This directory can also be specified by -R option.
output_dir/tmp directory : This directory is used to keep all the temporary files.
They serve as the checkpoint files so that SRAssembler can continue previous
assembling. If this directory is removed, SRAssembler must start from scratch in
the next run.

<a name="faq"/>
### FAQs
1. Who want to use this program
This program is designed for those who have whole genomic reads data, but interested in a few genes only. You can use this program to assemble the region of interest, instead of doing the whole genome de novo assembly, which may require a lot of computer resources.
2. Can I use multiple libraries
Yes, you can use -f option to assign multiple libraries (see manual).
