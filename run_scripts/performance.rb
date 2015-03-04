#------------------------------------------------------------------------------
# Performance Evaluation
#------------------------------------------------------------------------------
require_relative 'runDynamic'
include RunScripts

module RunScripts

    $secure_opts = {
      schemes: %w[tp],
      addrpar: true,
      rr_nc: true,
      use_way_part: true,
      split_mshr: true,
      split_rport: true
    }

    def cache_misses_spec
      outdir = "cache_misses_commit_f287e7"
      
      create_dir(
      outdir: "#{outdir}"
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1,
        l3_assoc: 1,
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 128,
        l3_assoc: 1
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 384,
        l3_assoc: 3
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 640,
        l3_assoc: 5
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 896,
        l3_assoc: 7
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1152,
        l3_assoc: 9
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1408,
        l3_assoc: 11
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1664,
        l3_assoc: 13
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1920,
        l3_assoc: 15
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 256,
        l3_assoc: 2
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 512,
        l3_assoc: 4
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 768,
        l3_assoc: 6
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1024,
        l3_assoc: 8
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1280,
        l3_assoc: 10
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1536,
        l3_assoc: 12
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 1792,
        l3_assoc: 14
      )
      
      single_qsub(
        outdir: "#{outdir}",
        cacheSize: 2048,
        l3_assoc: 16
      )
    end
    
    def perSet_misses_spec
      single_qsub(
        cacheSize: 1024
      )
    end
    
    def cache_misses_openssl
      single_qsub(
        cacheSize: 0,
        benchmarks: $openssl
      )
      
      single_qsub(
        cacheSize: 512,
        benchmarks: $openssl
      )
      
      single_qsub(
        cacheSize: 1024,
        benchmarks: $openssl
      )
      
      single_qsub(
        cacheSize: 2048,
        benchmarks: $openssl
      )
    end
    
    def perSet_misses_spec
      single_qsub(
        cacheSize: 1024,
        benchmarks: $openssl
      )
    end
      
    def baseline
      qsub_scaling(
        schemes: %w[none],
        cpus: %w[detailed],
        otherbench: $specint,
        maxinsts: 10**9
      )
    end

    def baseline_nocwf
      qsub_scaling(
        schemes: %w[none],
        cpus: %w[detailed],
        otherbench: %w[astar mcf],
        nocwf: true
      )
    end

    def scalability_qsub
        qsub_scaling $secure_opts.merge(
          maxinsts: 10**9
        )
    end

    def scalability_local
     parallel_local_scaling $secure_opts.merge(
       maxinsts: 10**9
     ) 
    end

    def breakdown

      qsub_fast(
        maxinsts: 10**9,
        nametag: "only_l2l3",
        addrpar: true,
        rr_l2l3: true,
        schemes: %w[none]
      )

      qsub_fast(
        maxinsts: 10**9,
        nametag: "only_membus",
        addrpar: true,
        rr_mem: true,
        schemes: %w[none]
      )

      qsub_fast(
        maxinsts: 10**9,
        nametag: "only_waypart",
        addrpar: true,
        waypart: true,
        schemes: %w[none]
      )

      qsub_fast(
        maxinsts: 10**9,
        nametag: "only_mc",
        addrpar: true,
        schemes: %w[tp]
      )

    end

    def coordination
      qsub_scaling $secure_opts.merge(
        maxinsts: 10**9,
        coordination: true,
        nametag: "coordinated",
      )
    end

    def double_tc
      qsub_scaling $secure_opts.merge(
        maxinsts: 10**9,
        nametag: "double_tc",
        skip2: true,
        skip3: true,
        numcpus: 4,
        numpids: 2,
        p0threadID: 0,
        p1threadID: 0,
        p2threadID: 1,
        p3threadID: 1
      )
    end
    
    def create_dir opts={}
      outdir = opts[:outdir]
      FileUtils.mkdir_p( "results/" + outdir ) unless File.directory?( "results/" + outdir )
      FileUtils.mkdir_p( "m5out/" + outdir ) unless File.directory?( "m5out/" + outdir )
      FileUtils.mkdir_p( "stdout/" + outdir ) unless File.directory?( "stdout/" + outdir )
      FileUtils.mkdir_p( "stderr/" + outdir ) unless File.directory?( "stderr/" + outdir )
    end
    
    def performance
      outdir = "performance_commit_d99096_2B"
      
      create_dir(
      outdir: "#{outdir}"
      )
      
      qsub_fast(
      maxinsts: 2*10**9,
      fastforward: 0,
      cacheSize: 2048,
      outdir: "#{outdir}",
      cpus: %w[detailed]
      )
    end

end
