#include "OrbitalAdder.h"
#include "OrbitalVector.h"
#include "Orbital.h"

extern MultiResolutionAnalysis<3> *MRA; // Global MRA

using namespace std;
using namespace Eigen;

OrbitalAdder::OrbitalAdder(double prec)
    : add(prec, MRA->getMaxScale()),
      grid(MRA->getMaxScale()) {
}

void OrbitalAdder::operator()(Orbital &phi_ab,
                              complex<double> a, Orbital &phi_a,
                              complex<double> b, Orbital &phi_b,
                              bool union_grid) {
    double prec = this->add.getPrecision();
    if (not union_grid and prec < 0.0) MSG_ERROR("Negative adaptive prec");
    if (phi_ab.hasReal() or phi_ab.hasImag()) MSG_ERROR("Orbital not empty");

    FunctionTreeVector<3> rvec;
    FunctionTreeVector<3> ivec;

    bool aHasReal = (fabs(a.real()) > MachineZero);
    bool aHasImag = (fabs(a.imag()) > MachineZero);
    bool bHasReal = (fabs(b.real()) > MachineZero);
    bool bHasImag = (fabs(b.imag()) > MachineZero);

    if (phi_a.hasReal() and aHasReal) rvec.push_back(a.real(), &phi_a.re());
    if (phi_b.hasReal() and bHasReal) rvec.push_back(b.real(), &phi_b.re());
    if (phi_a.hasImag() and aHasImag) rvec.push_back(-a.imag(), &phi_a.im());
    if (phi_b.hasImag() and bHasImag) rvec.push_back(-b.imag(), &phi_b.im());

    if (phi_a.hasReal() and aHasImag) ivec.push_back(a.imag(), &phi_a.re());
    if (phi_b.hasReal() and bHasImag) ivec.push_back(b.imag(), &phi_b.re());
    if (phi_a.hasImag() and aHasReal) ivec.push_back(a.real(), &phi_a.im());
    if (phi_b.hasImag() and bHasReal) ivec.push_back(b.real(), &phi_b.im());

    if (rvec.size() > 0) {
        if (union_grid) {
            phi_ab.allocReal();
            this->grid(phi_ab.re(), rvec);
            this->add(phi_ab.re(), rvec, 0);
        } else {
            phi_ab.allocReal();
            this->add(phi_ab.re(), rvec);
        }
    }
    if (ivec.size() > 0) {
        if (union_grid) {
            phi_ab.allocImag();
            this->grid(phi_ab.im(), ivec);
            this->add(phi_ab.im(), ivec, 0);
        } else {
            phi_ab.allocImag();
            this->add(phi_ab.im(), ivec);
        }
    }
}

void OrbitalAdder::operator()(Orbital &out,
                              std::vector<complex<double> > &coefs,
                              std::vector<Orbital *> &orbs,
                              bool union_grid) {
    double prec = this->add.getPrecision();
    if (not union_grid and prec < 0.0) MSG_ERROR("Negative adaptive prec");
    if (out.hasReal() or out.hasImag()) MSG_ERROR("Orbital not empty");
    if (coefs.size() != orbs.size()) MSG_ERROR("Invalid arguments");

    FunctionTreeVector<3> rvec;
    FunctionTreeVector<3> ivec;
    for (int i = 0; i < orbs.size(); i++) {
        bool cHasReal = (fabs(coefs[i].real()) > MachineZero);
        bool cHasImag = (fabs(coefs[i].imag()) > MachineZero);

        bool oHasReal = orbs[i]->hasReal();
        bool oHasImag = orbs[i]->hasImag();

        if (cHasReal and oHasReal) rvec.push_back(coefs[i].real(), &orbs[i]->re());
        if (cHasImag and oHasImag) rvec.push_back(-coefs[i].imag(), &orbs[i]->im());

        if (cHasImag and oHasReal) ivec.push_back(coefs[i].imag(), &orbs[i]->re());
        if (cHasReal and oHasImag) ivec.push_back(coefs[i].real(), &orbs[i]->im());
    }

    if (rvec.size() > 0) {
        if (union_grid) {
            out.allocReal();
            this->grid(out.re(), rvec);
            this->add(out.re(), rvec, 0);
        } else {
            out.allocReal();
            this->add(out.re(), rvec);
        }
    }
    if (ivec.size() > 0) {
        if (union_grid) {
            out.allocImag();
            this->grid(out.im(), ivec);
            this->add(out.im(), ivec, 0);
        } else {
            out.allocImag();
            this->add(out.im(), ivec);
        }
    }
}

void OrbitalAdder::operator()(OrbitalVector &out,
                              double a, OrbitalVector &inp_a,
                              double b, OrbitalVector &inp_b,
                              bool union_grid) {
    if (out.size() != inp_a.size()) MSG_ERROR("Invalid arguments");
    if (out.size() != inp_b.size()) MSG_ERROR("Invalid arguments");

    for (int i = 0; i < out.size(); i++) {
        Orbital &out_i = out.getOrbital(i);
        Orbital &aInp_i = inp_a.getOrbital(i);
        Orbital &bInp_i = inp_b.getOrbital(i);
        (*this)(out_i, a, aInp_i, b, bInp_i, union_grid);
    }
}

void OrbitalAdder::operator()(Orbital &out,
                              const VectorXd &c,
                              OrbitalVector &inp,
                              bool union_grid) {
    double prec = this->add.getPrecision();
    if (not union_grid and prec < 0.0) MSG_ERROR("Negative adaptive prec");
    if (c.size() != inp.size()) MSG_ERROR("Invalid arguments");
    if (out.hasReal() or out.hasImag()) MSG_ERROR("Output not empty");

    double thrs = MachineZero;
    FunctionTreeVector<3> rvec;
    FunctionTreeVector<3> ivec;
    for (int i = 0; i < inp.size(); i++) {
        double c_i = c(i);
        Orbital &phi_i = inp.getOrbital(i);
        if (phi_i.hasReal() and fabs(c_i) > thrs) rvec.push_back(c_i, &phi_i.re());
        if (phi_i.hasImag() and fabs(c_i) > thrs) ivec.push_back(c_i, &phi_i.im());
    }

    if (rvec.size() > 0) {
        if (union_grid) {
            out.allocReal();
            this->grid(out.re(), rvec);
            this->add(out.re(), rvec, 0);
        } else {
            out.allocReal();
            this->add(out.re(), rvec);
        }
    }
    if (ivec.size() > 0) {
        if (union_grid) {
            out.allocImag();
            this->grid(out.im(), ivec);
            this->add(out.im(), ivec, 0);
        } else {
            out.allocImag();
            this->add(out.im(), ivec);
        }
    }
}

/** In place rotation of orbital vector */
void OrbitalAdder::rotate_P(OrbitalVector &out, const MatrixXd &U) {
    OrbitalVector tmp(out);
    rotate_P(tmp, U, out);
    out.clear(true);    // Delete pointers
    out = tmp;          // Copy pointers
    tmp.clear(false);   // Clear pointers
}

void OrbitalAdder::rotate_P(OrbitalVector &out, const MatrixXd &U, OrbitalVector &phi) {
    if (U.cols() != phi.size()) MSG_ERROR("Invalid arguments");
    if (U.rows() < out.size()) MSG_ERROR("Invalid arguments");

    if(phi.size()%MPI_size)cout<<"NOT YET IMPLEMENTED"<<endl;

    int maxOrb =10;//to put into constants.h , or set dynamically?
    std::vector<complex<double> > U_Chunk;
    std::vector<Orbital *> orbChunk;
    for (int i = 0; i <phi.size(); i++) {
      out.getOrbital(i).clear(true);//
    }

    for (int i_Orb = MPI_rank; i_Orb < phi.size(); i_Orb+=MPI_size) {
      for (int iter = 0;  iter<MPI_size ; iter++) {
	int j_MPI=(MPI_size+iter-MPI_rank)%MPI_size;
	for (int j_Orb = j_MPI;  j_Orb < phi.size(); j_Orb+=MPI_size) {
	  
	  if(MPI_rank > j_MPI){
	    //send first bra, then receive ket
	    phi.getOrbital(i_Orb).send_Orbital(j_MPI, i_Orb);
	    out.getOrbital(j_Orb).Rcv_Orbital(j_MPI, j_Orb);
	  }else if(MPI_rank < j_MPI){
	    //receive first bra, then send ket
	    out.getOrbital(j_Orb).Rcv_Orbital(j_MPI, j_Orb);
	    phi.getOrbital(i_Orb).send_Orbital(j_MPI, i_Orb);
	  }else if(MPI_rank == j_MPI){//use phi directly
	  }
	  
	  //push orbital in vector (chunk)
	  if(MPI_rank == j_MPI){
	    orbChunk.push_back(&phi.getOrbital(j_Orb));
	  }else{
	    orbChunk.push_back(&out.getOrbital(j_Orb));
	  }
	  U_Chunk.push_back(U(i_Orb,j_Orb));	

	  if(orbChunk.size()>=maxOrb or iter >= MPI_size-1){
	    //Do the work for the chunk	  	  
	    this->inPlace(out.getOrbital(i_Orb),U_Chunk, orbChunk, false);//can start with empty orbital
	    U_Chunk.clear();
	    orbChunk.clear();
	  }
	}
      }
    }
    //TEMPORARY
    //broadcast results
    /*    for (int i = 0; i < phi.size(); i++) {      
      if(i%MPI_size==MPI_rank){
	for(int i_mpi = 0; i_mpi<MPI_size;i_mpi++){
	  if(i_mpi != MPI_rank){
	    out.getOrbital(i).send_Orbital(i_mpi, 54);
	  }
	}
      }else{
	out.getOrbital(i).Rcv_Orbital(i%MPI_size, 54);
      }
      }*/

}

void OrbitalAdder::rotate(OrbitalVector &out, const MatrixXd &U, OrbitalVector &inp) {
    if (U.cols() != inp.size()) MSG_ERROR("Invalid arguments");
    if (U.rows() < out.size()) MSG_ERROR("Invalid arguments");

    for (int i = 0; i < out.size(); i++) {
        const VectorXd &c = U.row(i);
        Orbital &out_i = out.getOrbital(i);
        (*this)(out_i, c, inp, false); // Adaptive grids
    }
}

/** In place rotation of orbital vector */
void OrbitalAdder::rotate(OrbitalVector &out, const MatrixXd &U) {
    OrbitalVector tmp(out);
    rotate(tmp, U, out);
    out.clear(true);    // Delete pointers
    out = tmp;          // Copy pointers
    tmp.clear(false);   // Clear pointers
}

void OrbitalAdder::inPlace(Orbital &out, complex<double> c, Orbital &inp) {
    Orbital tmp(out);//shallow copy
    (*this)(tmp, 1.0, out, c, inp, true); // Union grid
    out.clear(true);    // Delete pointers
    out = tmp;          // Copy pointers
    tmp.clear(false);   // Clear pointers
}

void OrbitalAdder::inPlace(Orbital &out,
                           vector<complex<double> > &c,
                           vector<Orbital *> &inp,
                           bool union_grid) {
    Orbital tmp(out);   // Shallow copy
    inp.push_back(&out);
    c.push_back(1.0);
    (*this)(tmp, c, inp, union_grid);
    out.clear(true);    // Delete pointers
    out = tmp;          // Copy pointers
    tmp.clear(false);   // Clear pointers
    inp.pop_back();     // Restore vector
    c.pop_back();       // Restore vector
   
}

void OrbitalAdder::inPlace(OrbitalVector &out, double c, OrbitalVector &inp) {
    if (out.size() != inp.size()) MSG_ERROR("Invalid arguments");

    for (int i = 0; i < out.size(); i++) {
        Orbital &out_i = out.getOrbital(i);
        Orbital &inp_i = inp.getOrbital(i);
        this->inPlace(out_i, c, inp_i);
    }
}

/** Orthogonalize the out orbital against inp
 */
void OrbitalAdder::orthogonalize(Orbital &out, Orbital &inp) {
    complex<double> inner_prod = out.dot(inp);
    double norm = inp.getSquareNorm();
    inPlace(out, -(inner_prod/norm), inp);
}

/** Orthogonalize the out orbital against all orbitals in inp
 */
void OrbitalAdder::orthogonalize(Orbital &out, OrbitalVector &inp) {
    for (int i = 0; i < inp.size(); i++) {
        Orbital &inp_i = inp.getOrbital(i);
        orthogonalize(out, inp_i);
    }
}

/** Orthogonalize all orbitals in out against the inp orbital
 *
 * Orbitals are NOT orthogonalized within the out set
 */
void OrbitalAdder::orthogonalize(OrbitalVector &out, Orbital &inp) {
    for (int i = 0; i < out.size(); i++) {
        Orbital &out_i = out.getOrbital(i);
        orthogonalize(out_i, inp);
    }
}

/** Orthogonalize all orbitals in out against all orbitals in inp
 *
 * Orbitals are NOT orthogonalized within the out set
 */
void OrbitalAdder::orthogonalize(OrbitalVector &out, OrbitalVector &inp) {
    for (int i = 0; i < out.size(); i++) {
        Orbital &out_i = out.getOrbital(i);
        orthogonalize(out_i, inp);
    }
}

