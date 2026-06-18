void bisinit(int32_t ipar[16],
	     double fpar[16],
	     int32_t wksize,
	     int32_t dsc,
	     bool& lp,
	     bool& rp,
	     double * wk)
{
  const double zero = 0;
  const double one = 1;
  
  //-----------------------------------------------------------------------
  //     some common initializations for the iterative solvers
  //-----------------------------------------------------------------------
  double zero = 0, one = 1;
  //
  //     ipar(1) = -2 inidcate that there are not enough space in the work
  //     array
  //
  if (ipar(4)<wksize)
    {
      ipar(1) = -2;
      ipar(4) = wksize;
      return;
    }

  if (ipar(2)>2)
    {
      lp = true;
      rp = true;
    }
  else if (ipar(2)==2)
    {
      lp = false;
      rp = true;
    }
  else if (ipar(2)==1)
    {
      lp = true;
      rp = false;
    }
  else
    {
      lp = false;
      rp = false;
    }
  if (ipar(3)==0) ipar(3) = dsc;
  
  //     .. clear the ipar elements used
  ipar(7) = 0;
  ipar(8) = 0;
  ipar(9) = 0;
  ipar(10) = 0;
  ipar(11) = 0;
  ipar(12) = 0;
  ipar(13) = 0;

  //     fpar(1) must be between (0, 1), fpar(2) must be positive,
  //     fpar(1) and fpar(2) can NOT both be zero
  //     Normally return ipar(1) = -4 to indicate any of above error

  if (fpar(1)<zero || fpar(1).ge.one || fpar(2)<zero ||
      &     (fpar(1)==zero && fpar(2)==zero))
    {
      if (ipar(1)==0)
	{
	  ipar(1) = -4;
	  return;
	}
      else
	{
	  fpar(1) = 1.0D-6;
	  fpar(2) = 1.0D-16;
	}
    }
  //  c     .. clear the fpar elements
      do i = 3, 10
         fpar(i) = zero
      enddo
      if (fpar(11)<zero) fpar(11) = zero
			   //c     .. clear the used portion of the work array to zero
      do i = 1, wksize
         wk(i) = zero
      enddo
	   //c
      return
	   // c-----end-of-bisinit
	   //      end
#if 0

     call mgsro(full,n,n,lb,jp1,fpar(11),w(iv+1),w(ihm+1),
     $     ipar(12))
      if (ipar(12)<0) {
         ipar(1) = -3
         goto 80
      }
      beta = w(ihm+jp1)
c
c     incomplete factorization (QR factorization through Givens rotations)
c     (1) apply previous rotations [(lb-1) of them]
c     (2) generate a new rotation
c
      if (full) {
         w(ihm+jp1) = w(ihm+j0) * w(is+jp1)
         w(ihm+j0) = w(ihm+j0) * w(ic+jp1)
      }
      i = j0
      do while (i.ne.j)
         k = i+1
         if (k>lb) k = k - lb
         c = w(ic+i)
         s = w(is+i)
         alpha = w(ihm+i)
         w(ihm+i) = c * alpha + s * w(ihm+k)
         w(ihm+k) = c * w(ihm+k) - s * alpha
         i = k
      enddo
      call givens(w(ihm+j), beta, c, s)
      if (full) {
         fpar(11) = fpar(11) + 6 * lb
      }
      else
	{
	  fpar(11) = fpar(11) + 6 * j
	   }
c
c     detect whether diagonal element of this column is zero
c
      if (abs(w(ihm+j))<deps) {
         ipar(1) = -3
         goto 80
      }
      w(ihd+j) = one / w(ihm+j)
      w(ic+j) = c
      w(is+j) = s
c
c     update the W's (the conjugate directions) -- essentially this is one
c     step of triangular solve.
c
      ptrw = iw+(j-1)*n + 1
      if (full) {
         do i = j+1, lb
            alpha = -w(ihm+i)*w(ihd+i)
            ptr = iw+(i-1)*n+1
            do ii = 0, n-1
               w(ptrw+ii) = w(ptrw+ii) + alpha * w(ptr+ii)
            enddo
         enddo
      }
      do i = 1, j-1
         alpha = -w(ihm+i)*w(ihd+i)
         ptr = iw+(i-1)*n+1
         do ii = 0, n-1
            w(ptrw+ii) = w(ptrw+ii) + alpha * w(ptr+ii)
         enddo
      enddo
c
c     update the solution to the linear system
c
      alpha = psi * c * w(ihd+j)
      psi = - s * psi
      do i = 1, n
         sol(i) = sol(i) + alpha * w(ptrw-1+i)
      enddo
      if (full) {
         fpar(11) = fpar(11) + lb * (n+n)
      }
	 else
	   {
         fpar(11) = fpar(11) + j * (n+n)
      }
c
c     determine whether to continue,
c     compute the desired error/residual norm
c
      ipar(7) = ipar(7) + 1
      fpar(5) = abs(psi)
      if (ipar(3)==999) {
         ipar(1) = 10
         ipar(8) = -1
         ipar(9) = 1
         ipar(10) = 6
         return
      }
      if (ipar(3)<0) {
         alpha = abs(alpha)
         if (ipar(7)==2 && ipar(3)==-1) {
            fpar(3) = alpha*sqrt(distdot(n, w(ptrw), 1, w(ptrw), 1))
            fpar(4) = fpar(1) * fpar(3) + fpar(2)
            fpar(6) = fpar(3)
	 }
         else
	   {
            fpar(6) = alpha*sqrt(distdot(n, w(ptrw), 1, w(ptrw), 1))
         }
         fpar(11) = fpar(11) + 2 * n
      else
         fpar(6) = fpar(5)
      endif
      if (ipar(1).ge.0 && fpar(6)>fpar(4) && (ipar(6).le.0
     +     || ipar(7)<ipar(6))) goto 30
 70   if (ipar(3)==999 && ipar(11)==0) goto 30
c
c     clean up the iterative solver
c
 80   fpar(7) = zero
      if (fpar(3).ne.zero && fpar(6).ne.zero &&
     +     ipar(7)>ipar(13))
     +     fpar(7) = log10(fpar(3) / fpar(6)) / dble(ipar(7)-ipar(13))
      if (ipar(1)>0) {
         if (ipar(3)==999 && ipar(11).ne.0) {
            ipar(1) = 0
         else if (fpar(6).le.fpar(4)) {
            ipar(1) = 0
         else if (ipar(6)>0 && ipar(7).ge.ipar(6)) {
            ipar(1) = -1
         else
            ipar(1) = -10
         endif
      endif
      return
      end

#if 0

subroutine givens(x,y,c,s)
      real*8 x,y,c,s
c-----------------------------------------------------------------------
c     Given x and y, this subroutine generates a Givens' rotation c, s.
c     And apply the rotation on (x,y) ==> (sqrt(x**2 + y**2), 0).
c     (See P 202 of "matrix computation" by Golub and van Loan.)
c-----------------------------------------------------------------------
      real*8 t,one,zero
      parameter (zero=0.0D0,one=1.0D0)
c
      if (x==zero && y==zero) {
         c = one
         s = zero
      else if (abs(y)>abs(x)) {
         t = x / y
         x = sqrt(one+t*t)
         s = sign(one / x, y)
         c = t*s
      else if (abs(y).le.abs(x)) {
         t = y / x
         y = sqrt(one+t*t)
         c = sign(one / y, x)
         s = t*c
      else
c
c     X or Y must be an invalid floating-point number, set both to zero
c
         x = zero
         y = zero
         c = one
         s = zero
      endif
      x = abs(x*y)
c
c     end of givens
c
      return
      end
	      
void fgmres(int64_t n, void * rhs,  void * sol, int32_t ipar[], double fpar[], w)
      implicit none
      integer n, ipar(16)
      real*8 rhs(n), sol(n), fpar(16), w(*)
c-----------------------------------------------------------------------
c     This a version of FGMRES implemented with reverse communication.
c
c     ipar(5) == the dimension of the Krylov subspace
c
c     the space of the `w' is used as follows:
c     >> V: the bases for the Krylov subspace, size n*(m+1);
c     >> W: the above bases after (left-)multiplying with the
c     right-preconditioner inverse, size m*n;
c     >> a temporary vector of size n;
c     >> the Hessenberg matrix, only the upper triangular portion
c     of the matrix is stored, size (m+1)*m/2 + 1
c     >> three vectors, first two are of size m, they are the cosine
c     and sine of the Givens rotations, the third one holds the
c     residuals, it is of size m+1.
c
c     TOTAL SIZE REQUIRED == n*(2m+1) + (m+1)*m/2 + 3*m + 2
c     Note: m == ipar(5). The default value for this is 15 if
c     ipar(5) <= 1.
c-----------------------------------------------------------------------
c     external functions used
c
      real*8 distdot
      external distdot
c
      real*8 one, zero
      parameter(one=1.0D0, zero=0.0D0)
c
c     local variables, ptr and p2 are temporary pointers,
c     hess points to the Hessenberg matrix,
c     vc, vs point to the cosines and sines of the Givens rotations
c     vrn points to the vectors of residual norms, more precisely
c     the right hand side of the least square problem solved.
c
      integer i,ii,idx,iz,k,m,ptr,p2,hess,vc,vs,vrn
      real*8 alpha, c, s
      logical lp, rp
      save
c
c     check the status of the call
c
      if (ipar(1).le.0) ipar(10) = 0
      goto (10, 20, 30, 40, 50, 60) ipar(10)
c
c     initialization
c
      if (ipar(5).le.1) {
         m = 15
      else
         m = ipar(5)
      endif
      idx = n * (m+1)
      iz = idx + n
      hess = iz + n*m
      vc = hess + (m+1) * m / 2 + 1
      vs = vc + m
      vrn = vs + m
      i = vrn + m + 1
      call bisinit(ipar,fpar,i,1,lp,rp,w)
      if (ipar(1)<0) return
c
c     request for matrix vector multiplication A*x in the initialization
c
 100  ipar(1) = 1
      ipar(8) = n+1
      ipar(9) = 1
      ipar(10) = 1
      k = 0
      do ii = 1, n
         w(ii+n) = sol(ii)
      enddo
      return
 10   ipar(7) = ipar(7) + 1
      ipar(13) = ipar(13) + 1
      fpar(11) = fpar(11) + n
      if (lp) {
         do i = 1, n
            w(n+i) = rhs(i) - w(i)
         enddo
         ipar(1) = 3
         ipar(10) = 2
         return
      else
         do i = 1, n
            w(i) = rhs(i) - w(i)
         enddo
      endif
c
 20   alpha = sqrt(distdot(n,w,1,w,1))
      fpar(11) = fpar(11) + n + n
      if (ipar(7)==1 && ipar(3).ne.999) {
         if (abs(ipar(3))==2) {
            fpar(4) = fpar(1) * sqrt(distdot(n,rhs,1,rhs,1)) + fpar(2)
            fpar(11) = fpar(11) + 2*n
         else
            fpar(4) = fpar(1) * alpha + fpar(2)
         endif
         fpar(3) = alpha
      endif
      fpar(5) = alpha
      w(vrn+1) = alpha
      if (alpha.le.fpar(4) && ipar(3).ge.0 && ipar(3).ne.999) {
         ipar(1) = 0
         fpar(6) = alpha
         goto 300
      endif
      alpha = one / alpha
      do ii = 1, n
         w(ii) = w(ii) * alpha
      enddo
      fpar(11) = fpar(11) + n
c
c     request for (1) right preconditioning
c     (2) matrix vector multiplication
c     (3) left preconditioning
c
 110  k = k + 1
      if (rp) {
         ipar(1) = 5
         ipar(8) = k*n - n + 1
         ipar(9) = iz + ipar(8)
         ipar(10) = 3
         return
      else
         do ii = 0, n-1
            w(iz+k*n-ii) = w(k*n-ii)
         enddo
      endif
c
 30   ipar(1) = 1
      if (rp) {
         ipar(8) = ipar(9)
      else
         ipar(8) = (k-1)*n + 1
      endif
      if (lp) {
         ipar(9) = idx + 1
      else
         ipar(9) = 1 + k*n
      endif
      ipar(10) = 4
      return
c
 40   if (lp) {
         ipar(1) = 3
         ipar(8) = ipar(9)
         ipar(9) = k*n + 1
         ipar(10) = 5
         return
      endif
c
c     Modified Gram-Schmidt orthogonalization procedure
c     temporary pointer 'ptr' is pointing to the current column of the
c     Hessenberg matrix. 'p2' points to the new basis vector
c
 50   ptr = k * (k - 1) / 2 + hess
      p2 = ipar(9)
      ipar(7) = ipar(7) + 1
      call mgsro(false,n,n,k+1,k+1,fpar(11),w,w(ptr+1),
     $     ipar(12))
      if (ipar(12)<0) goto 200
c
c     apply previous Givens rotations and generate a new one to eliminate
c     the subdiagonal element.
c
      p2 = ptr + 1
      do i = 1, k-1
         ptr = p2
         p2 = p2 + 1
         alpha = w(ptr)
         c = w(vc+i)
         s = w(vs+i)
         w(ptr) = c * alpha + s * w(p2)
         w(p2) = c * w(p2) - s * alpha
      enddo
      call givens(w(p2), w(p2+1), c, s)
      w(vc+k) = c
      w(vs+k) = s
      p2 = vrn + k
      alpha = - s * w(p2)
      w(p2) = c * w(p2)
      w(p2+1) = alpha
      fpar(11) = fpar(11) + 6 * k
c
c     end of one Arnoldi iteration, alpha will store the estimated
c     residual norm at current stage
c
      alpha = abs(alpha)
      fpar(5) = alpha
      if (k<m && .not.(ipar(3).ge.0 && alpha.le.fpar(4))
     +      && (ipar(6).le.0 || ipar(7)<ipar(6))) goto 110
c
c     update the approximate solution, first solve the upper triangular
c     system, temporary pointer ptr points to the Hessenberg matrix,
c     p2 points to the right-hand-side (also the solution) of the system.
c
 200  ptr = hess + k * (k + 1 ) / 2
      p2 = vrn + k
      if (w(ptr)==zero) {
c
c     if the diagonal elements of the last column is zero, reduce k by 1
c     so that a smaller trianguler system is solved [It should only
c     happen when the matrix is singular!]
c
         k = k - 1
         if (k>0) {
            goto 200
         else
            ipar(1) = -3
            ipar(12) = -4
            goto 300
         endif
      endif
      w(p2) = w(p2) / w(ptr)
      do i = k-1, 1, -1
         ptr = ptr - i - 1
         do ii = 1, i
            w(vrn+ii) = w(vrn+ii) - w(p2) * w(ptr+ii)
         enddo
         p2 = p2 - 1
         w(p2) = w(p2) / w(ptr)
      enddo
c
      do i = 0, k-1
         ptr = iz+i*n
         do ii = 1, n
            sol(ii) = sol(ii) + w(p2)*w(ptr+ii)
         enddo
         p2 = p2 + 1
      enddo
      fpar(11) = fpar(11) + 2*k*n + k*(k+1)
c
c     process the complete stopping criteria
c
      if (ipar(3)==999) {
         ipar(1) = 10
         ipar(8) = -1
         ipar(9) = idx + 1
         ipar(10) = 6
         return
      else if (ipar(3)<0) {
         if (ipar(7).le.m+1) {
            fpar(3) = abs(w(vrn+1))
            if (ipar(3)==-1) fpar(4) = fpar(1)*fpar(3)+fpar(2)
         endif
         fpar(6) = abs(w(vrn+k))
      else if (ipar(3).ne.999) {
         fpar(6) = fpar(5)
      endif
c
c     do we need to restart ?
c
 60   if (ipar(12).ne.0) {
         ipar(1) = -3
         goto 300
      endif
      if ((ipar(7)<ipar(6) || ipar(6).le.0)&&
     +     ((ipar(3)==999&&ipar(11)==0) ||
     +     (ipar(3).ne.999&&fpar(6)>fpar(4)))) goto 100
c
c     termination, set error code, compute convergence rate
c
      if (ipar(1)>0) {
         if (ipar(3)==999 && ipar(11)==1) {
            ipar(1) = 0
         else if (ipar(3).ne.999 && fpar(6).le.fpar(4)) {
            ipar(1) = 0
         else if (ipar(7).ge.ipar(6) && ipar(6)>0) {
            ipar(1) = -1
         else
            ipar(1) = -10
         endif
      endif
 300  if (fpar(3).ne.zero && fpar(6).ne.zero &&
     $     ipar(7)>ipar(13)) {
         fpar(7) = log10(fpar(3) / fpar(6)) / dble(ipar(7)-ipar(13))
      else
         fpar(7) = zero
      endif
      return
      end
#endif
#endif
