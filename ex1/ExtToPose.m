function [R,T]=ExtToPose(RR,TT)
  R=RR';
  T=-TT*R;
end