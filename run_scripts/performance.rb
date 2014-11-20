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
      single_qsub(
        cacheSize: 0
      )
      
      single_qsub(
        cacheSize: 512
      )
      
      single_qsub(
        cacheSize: 1024
      )
      
      single_qsub(
        cacheSize: 2048
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
    
    def performance
      qsub_fast(
      maxinsts: 10**9,
      fastforward: 10**9,
      fixaddr: true,
      cpus: %w[detailed]
      )
    end

end
