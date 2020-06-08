using FFTW
using Plots
using Printf

@printf("[+] Center frequency %s, sample rate %s\n", ARGS[2], ARGS[3]);
freq       = parse(Int, ARGS[2])
samplerate = parse(Int, ARGS[3])

@printf("[+] Opening file: %s\n", ARGS[1])

finfo = stat(ARGS[1])

sampleCount = trunc(Int, finfo.size/2)

samples = Array{Int16, 1}(undef, sampleCount)

f = open(ARGS[1])

@printf("[+] Reading %d samples in\n", sampleCount)
read!(f, samples)

close(f)

@printf("[+] Done. Doing fft...\n")

fftTemp = fft(samples)
@printf("[+] Done.\n")

fftdB = Array{Float64, 1}(undef, sampleCount)

max = -100.0
maxIdx = 0
for x in 1:sampleCount
    local xreal
    if x > sampleCount/2
        xreal = x - trunc(Int, sampleCount/2)
    else
        xreal = trunc(Int, sampleCount/2) + x
    end

    i = imag(fftTemp[xreal])
    r = real(fftTemp[xreal])
    fftdB[x] = 10*log10((i*i+r*r)/(sampleCount*sampleCount))

    if fftdB[xreal] > max
        global max = fftdB[xreal]
        global maxIdx = xreal
    end

    if fftdB[x] > 20.0
        @printf("[+] idx: %d val: %f\n", x, fftdB[x])
    end

end

calcFreq = (freq-samplerate/2)+((maxIdx-1)*(samplerate/sampleCount))

@printf("[+] max => fftdB[%d]=%f @%f Hz\n", maxIdx, max, calcFreq)

domain = 1:sampleCount

gr();

plot(domain, fftdB, label="dB")

savefig("fft_graph.png")
