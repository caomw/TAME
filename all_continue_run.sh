for i in {1..9}; do ./tri-match -t smat -H input/BioGRID/yeast_net.smat -G input/BioGRID/human_net.smat -S input/BioGRID/human-yeast.smat --iter 0 -x seqsim -C TAME_X.mat -a 0.$i -b 0; done