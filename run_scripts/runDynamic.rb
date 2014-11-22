#!/usr/bin/ruby

require 'fileutils'
require 'colored'

module RunScripts
#directories
$gem5home = Dir.new(Dir.pwd)
$specint_dir = ("benchmarks/spec2k6bin/specint")
$openssl_dir = ("tests/openssl-1.0.1g/test")
$scriptgen_dir = Dir.new(Dir.pwd+"/scriptgen")

#Gem5 options
$fastforward = 10**9
$maxinsts = 10**8
$maxtick = 2*10**15 
$cpus = %w[timing detailed] #timing is also available
$schemes = %w[static static0 static25 static75 static100 c_dynamic f_dynamic]

#benchmarks
$specinvoke = { 
   #"perlbench"  => "'#{$specint_dir}/perlbench -I#{$specint_dir}/perldepends -I#{$specint_dir}/lib #{$specint_dir} pack.pl'",
    "bzip2"      => "'#{$specint_dir}/bzip2 #{$specint_dir}/input.source 280'",
    "gcc"        => "'#{$specint_dir}/gcc #{$specint_dir}/200.in -o results/200.s'",
    "mcf"        => "'#{$specint_dir}/mcf #{$specint_dir}/inp.in'",
    "gobmk"      => "'#{$specint_dir}/gobmk --quiet --mode gtp --gtp-input #{$specint_dir}/13x13.tst'",
    "hmmer"      => "'#{$specint_dir}/hmmer #{$specint_dir}/nph3.hmm #{$specint_dir}/swiss41'",
    "sjeng"      => "'#{$specint_dir}/sjeng #{$specint_dir}/ref.txt'",
    "libquantum" => "'#{$specint_dir}/libquantum 1397 8'",
    "h264ref"    => "'#{$specint_dir}/h264ref -d #{$specint_dir}/foreman_ref_encoder_baseline.cfg'",
   #"omnetpp"    => "'#{$specint_dir}/omnetpp #{$specint_dir}/omnetpp.ini'",
    "astar"      => "'#{$specint_dir}/astar #{$specint_dir}/BigLakes2048.cfg'",
    "xalan"      => "'#{$specint_dir}/Xalan -v #{$specint_dir}/t5.xml #{$specint_dir}/xalanc.xsl'"  
}
$specint = $specinvoke.keys.sort

$synthinvoke = {
    "hardstride1" => "#{$synthbench_dir}hardstride -d #{$duration} -p 1",
    "randmem1"    => "#{$synthbench_dir}randmem -d #{$duration} -p 1",
    #"randmem10"   => "#{$synthbench_dir}randmem -d #{$duration} -p 10",
    #"randmem100"  => "#{$synthbench_dir}randmem -d #{$duration} -p 100",
    "nothing"     => "#{$synthbench_dir}nothing"
}
$synthb = $synthinvoke.keys.sort

$opensslinvoke = {
    "AES"          => "'#{$openssl_dir}/asn1test'",
    "Blowfish"     => "'#{$openssl_dir}/bftest'",
    "CAST5"        => "'#{$openssl_dir}/casttest'",
    "DES"          => "'#{$openssl_dir}/destest'",
    "HMAC-MD5"     => "'#{$openssl_dir}/hmactest'",
    "IDEA"         => "'#{$openssl_dir}/ideatest'",
    "RC2"          => "'#{$openssl_dir}/rc2test'",
    "RC4"          => "'#{$openssl_dir}/rc4test'",
    "RC5"          => "'#{$openssl_dir}/rc5test'",
    "MD2"          => "'#{$openssl_dir}/md2test'",
    "MD4"          => "'#{$openssl_dir}/md4test'",
    "MD5"          => "'#{$openssl_dir}/md5test'",
    "MDC-2"        => "'#{$openssl_dir}/mdc2test'",
    "RSA"          => "'#{$openssl_dir}/rsa_test'",
    "SHA-0"        => "'#{$openssl_dir}/shatest'",
    "SHA-1"        => "'#{$openssl_dir}/sha1test'",
    "SHA-256-224"  => "'#{$openssl_dir}/sha256t'",
    "SHA-512-384"  => "'#{$openssl_dir}/sha512t'",
    "Whirlpool"    => "'#{$openssl_dir}/wp_test'",
}
$openssl = $opensslinvoke.keys.sort

$staticinvoke = {
    "static"    => "8",
    "static0"    => "1",
    "static25"   => "4",
    "static75"   => "12",
    "static100"  => "15",
}

def invoke( name )
    $specinvoke[name] || $synthinvoke[name] || $opensslinvoke[name] || $staticinvoke[name]
end

def sav_script( cpu, scheme, p0, options = {} ) 

    options = {
        l3config: "shared",
        cacheSize: 1024,
        runmode: :qsub,
        maxinsts: $maxinsts,
        fastforward: $fastforward,
        result_dir: "results"
    }.merge options

    cacheSize  = options[:cacheSize]
    # workloads to run on p1-p3
    p1         = options[:p1]
    p2         = options[:p2]
    p3         = options[:p3]
    # options for c_dynamic cache
    h_min      = options[:h_min]
    th         = options[:th]
    # Results directory
    result_dir = options[:result_dir]
    # shared or private l3
    l3config   = options[:l3config]
    # runmode can be qsub: to submit jobs, local: to run the test locally, or 
    # none: to generate the scripts without running them
    runmode    = options[:runmode]
    # maximum number of instructions
    maxinsts   = options[:maxinsts]
    # number of instructions to fastforward,
    # 0 removes --fastforward from the script
    fastforward= options[:fastforward]
    # Use gem5.debug instead of gem5.fast
    debug = options[:debug] ||
        options[:cacheDebug] ||
        options[:gdb] ||
        options[:memdebug] ||
        options[:mmuDebug]
    

    filename  = "#{scheme}"
    filename += "_#{h_min}_#{th}" if scheme == "c_dynamic"
    filename += "_#{cpu}_#{p0}"
    filename += "_#{p1}" unless p1.nil?
    filename += "_#{p2}" unless p2.nil?
    filename += "_#{p3}" unless p3.nil?
    filename += "_c#{cacheSize}kB"

    filename = "#{options[:nametag]}_"+filename if options[:nametag]
    filename = options[:filename] unless options[:filename].nil?
   
    numpids = [p0,p1,p2,p3].inject(0){|sum,i| ( i.nil? && sum ) || sum+1}
    numpids = options[:numpids] unless options[:numpids].nil?

    FileUtils.mkdir_p( result_dir ) unless File.directory?( result_dir )

    script = File.new($scriptgen_dir.path+"/run_#{filename}","w+")
    script.puts("#!/bin/bash")
    script.puts("build/ARM/gem5.fast \\") unless debug
    script.puts("build/ARM/gem5.debug \\") if debug
    script.puts("    --remote-gdb-port=0 \\")
    script.puts("    --stats-file=#{filename}_stats.txt \\")
    script.puts("    configs/dramsim2/dynamic_cache.py \\")
    script.puts("    --cpu-type=#{cpu} \\")
    script.puts("    --fixaddr \\") if options[:fixaddr]
    script.puts("    --c_dynamic_cache \\") if scheme != "f_dynamic"
    script.puts("    --f_dynamic_cache \\") if scheme == "f_dynamic"
    script.puts("    --static_cache \\") if scheme != "f_dynamic" && scheme != "c_dynamic"
    script.puts("    --L_assoc=#{invoke(scheme)} \\") if scheme != "f_dynamic" && scheme != "c_dynamic"
    script.puts("    --H_min=#{h_min} \\") if scheme == "c_dynamic"
    script.puts("    --th_inc=-0.00#{th} \\") if scheme == "c_dynamic"
    script.puts("    --th_dec=0.00#{th} \\") if scheme == "c_dynamic"
    script.puts("    --print_misses \\") if options[:print_misses]
    script.puts("    --print_perSet_misses \\") if options[:print_perSet_misses]
    script.puts("    --caches \\")
    script.puts("    --l2cache \\")
    unless cacheSize == 0
        script.puts("    --l3cache \\")
        script.puts("    --l3_size=#{cacheSize}kB\\")
        script.puts("    --l3config=#{l3config} \\")
    end
    script.puts("    --l2trace \\") if cacheSize == 0
    script.puts("    --fast-forward=#{fastforward} \\") unless fastforward == 0
    script.puts("    --maxinsts=#{maxinsts} \\")
    script.puts("    --maxtick=#{$maxtick} \\")

    script.puts("    --numpids=#{numpids} \\")
    unless options[:numcpus].nil?
      script.puts("    --numcpus=#{options[:numcpus]} \\")
    end
    script.puts("    --p0=#{invoke(p0)}\\")
    script.puts("    --p1=#{invoke(p1)}\\") unless p1.nil?
    script.puts("    --p2=#{invoke(p2)}\\") unless p2.nil?
    script.puts("    --p3=#{invoke(p3)}\\") unless p3.nil?

    script.puts("    > #{result_dir}/stdout_#{filename}.out")
    script_abspath = File.expand_path(script.path)
    script.close

    FileUtils.mkdir_p( "stderr" ) unless File.directory?( "stderr" )
    FileUtils.mkdir_p( "stdout" ) unless File.directory?( "stdout" )
    
    sleep(2)
    
    if runmode == :qsub
        success = system "qsub -wd #{$gem5home.path} -e stderr/ -o stdout/ #{script_abspath}"
    end
    puts "#{filename}".magenta if runmode == :local
    success = system "sh #{script_abspath}" if runmode == :local
    #success = true
    [success,filename]
end

def iterate_and_submit opts={}, &block
    o = {
        cpus: $cpus,
        schemes: $schemes,
        benchmarks: $specint,
        runmode: :local,
        threads: 1,
        h_mins: %w[4 8],
        ths: %w[1 2 3],
    }.merge opts

    o[:otherbench] = o[:benchmarks] if o[:otherbench].nil?

    f = []

    submit = block_given? ?
      block :
      ( lambda do |cpu, scheme, param, p0, other|
          r = sav_script(cpu, scheme, p0, param.merge(p1: other))
          (r[0] && [] ) || r[1]
        end
      )
      
    o[:cpus].product(o[:schemes]).each do |cpu, scheme|
      o[:benchmarks].product(o[:otherbench]).each_slice(o[:threads]) do |i|
        threads=[]
        i.each do |p0,other|
          if scheme == "c_dynamic"
            o[:h_mins].product(o[:ths]).each do |h_min, th|
              o.merge!(h_min: h_min, th: th) 
             submit.call(cpu, scheme, o, p0, other).flatten 
            end
          else
            submit.call(cpu, scheme, o, p0, other).flatten 
          end  
        end
        threads.each { |t| t.join }
      end
    end
    puts f.flatten.to_s.red
end

def parallel_local opts={}
  iterate_and_submit opts
end

def qsub_fast opts={}
  iterate_and_submit ({runmode: :qsub}).merge(opts)
end

def single opts={}
    o = {
        cpus: %w[detailed timing],
        cacheSize: 4096,
        schemes: ["none"],
        benchmarks: $specint,
        runmode: :local,
        threads: 4,
        l3config: "private",
        print_misses: true,
        print_perSet_misses: true,
        fastforward: 0,
        maxinsts: 2*10**9,
    }.merge opts

    f = []

    o[:cpus].product(o[:schemes], o[:benchmarks]).each_slice(o[:threads]) do |i|
      t={}
      i.each do |cpu,scheme,p0|
        t[i] = Thread.new do
          r=sav_script(cpu, scheme, p0, o)
          f << r[1] unless r[0]
        end
      end
      t.each { |_,v| v.join }
    end
    puts f.flatten.to_s.red
end

def single_qsub opts={}
  single opts.merge(runmode: :qsub)
end

def parallel_local_scaling opts={}
  opts = {otherbench: %w[astar]}.merge opts
  iterate_and_submit(opts) do |cpu, scheme, param, p0, other|
    f = []
    #2
    p = param.merge(p1: other)
    p = p.merge coordinate(n:2) if opts[:coordination]
    r = opts[:skip2]? [true,""] : sav_script(cpu, scheme, p0, p)
    f << r[1] unless r[0]
    #3
    p = p.merge(p2: other)
    p = p.merge coordinate(n:3) if opts[:coordination]
    r = opts[:skip3]? [true,""] : sav_script(cpu, scheme, p0, p)
    f << r[1] unless r[0]
    #4
    p = p.merge(p3: other)
    p = p.merge coordinate(n:4) if opts[:coordination]
    r = opts[:skip4]? [true,""] : sav_script(cpu, scheme, p0, p)
    f << r[1] unless r[0]
    f
  end
end

def qsub_scaling opts = {}
  parallel_local_scaling opts.merge(runmode: :qsub)
end

end
