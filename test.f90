module mero_fortran
  abstract interface 
     ! send array
     subroutine send_array_int(array, array_size, block_size, & 
          block_count, idhi, idlo) bind(C)
       use iso_c_binding
       implicit none
       integer(kind=C_INT) :: array(*)
       integer(kind=C_INT),value :: array_size
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT)  :: block_count
       integer(kind=C_INT64_T) :: idhi
       integer(kind=C_INT64_T) :: idlo
     end subroutine send_array_int

     ! send array
     subroutine send_array_real(array, array_size, block_size, & 
          block_count, idhi, idlo) bind(C)
       use iso_c_binding
       implicit none
       real(kind=C_FLOAT) :: array(*)
       integer(kind=C_INT),value :: array_size
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT)  :: block_count
       integer(kind=C_INT64_T) :: idhi
       integer(kind=C_INT64_T) :: idlo
     end subroutine send_array_real

     ! send array
     subroutine send_array_long(array, array_size, block_size, & 
          block_count, idhi, idlo) bind(C)
       use iso_c_binding
       implicit none
       integer(kind=C_LONG) :: array(*)
       integer(kind=C_INT),value :: array_size
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT)  :: block_count
       integer(kind=C_INT64_T) :: idhi
       integer(kind=C_INT64_T) :: idlo
     end subroutine send_array_long

     ! send array
     subroutine send_array_double(array, array_size, block_size, & 
          block_count, idhi, idlo) bind(C)
       use iso_c_binding
       implicit none
       real(kind=C_DOUBLE) :: array(*)
       integer(kind=C_INT),value :: array_size
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT)  :: block_count
       integer(kind=C_INT64_T) :: idhi
       integer(kind=C_INT64_T) :: idlo
     end subroutine send_array_double

     subroutine recieve_array(array_recieved, array_length, block_size, &
          block_count, idhi, idlo ) bind(C)
       use iso_c_binding
       implicit none
       type(C_PTR) :: array_recieved 
       integer(kind=C_INT),value :: array_length
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT),value :: block_count
       integer(kind=C_INT64_T),value :: idhi
       integer(kind=C_INT64_T),value :: idlo
     end subroutine recieve_array
     
     subroutine start_clovis() bind(C)
     end subroutine start_clovis 

     subroutine finish_clovis() bind(C)
     end subroutine finish_clovis
  end interface
end module

program test_fortran_c
use mero_fortran
implicit none
procedure(start_clovis) :: mero_start
procedure(finish_clovis) :: mero_finish


call mero_start()

! vector array tests
call test_int()
call test_long()
call test_float()
call test_double()

! (2d array tests
call test_2d_int()
call test_2d_long()
call test_2d_float()
call test_2d_double()

call mero_finish()

end program test_fortran_c

subroutine test_int()
use mero_fortran
use iso_c_binding

implicit none

integer(kind=C_INT), allocatable :: array(:) ! the array to fill
type(C_PTR) :: p
integer(C_INT), pointer :: array_recieved(:)
integer :: i                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_int) :: mero_send_array_int
procedure(recieve_array) :: mero_recieve_array_int

block_size = 4096

allocate(array(100))
allocate(array_recieved(100))
array = 1.
do i = 1,100
  array(i) = i
enddo
array_recieved = 0.

call mero_send_array_int(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_int(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array)])

do i = 1, size(array)
   if ( array(i) .ne. array_recieved(i) ) then
      write(*,*) 'Arrays not he same', array(i), array_recieved(i)
   endif
enddo

write(*,*) '--- int test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_int

subroutine test_2d_int()
use mero_fortran
use iso_c_binding

implicit none

integer(kind=C_INT), allocatable :: array(:,:) ! the array to fill
type(C_PTR) :: p
integer(C_INT), pointer :: array_recieved(:,:)
integer :: i,j                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_int) :: mero_send_array_int
procedure(recieve_array) :: mero_recieve_array_int

block_size = 4096

allocate(array(100,100))
allocate(array_recieved(100,100))

array = 1.

do i = 1,100
  do j = 1,100
    array(i,j) = j + (i-1)*100
  enddo
enddo

array_recieved = 0.

call mero_send_array_int(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_int(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array,1),size(array,2)])

do i = 1, 100
  do j = 1, 100
    if ( array(i,j) .ne. array_recieved(i,j) ) then
      write(*,*) 'Arrays not he same', array(i,j), array_recieved(i,j)
    endif
  enddo
enddo

write(*,*) '--- int 2d test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_2d_int

subroutine test_long()
use mero_fortran
use iso_c_binding

implicit none

integer(kind=C_LONG), allocatable :: array(:) ! the array to fill
type(C_PTR) :: p
integer(C_LONG), pointer :: array_recieved(:)
integer(kind=C_LONG) :: i                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_long) :: mero_send_array_long
procedure(recieve_array) :: mero_recieve_array_long

block_size = 4096

allocate(array(100))
allocate(array_recieved(100))
array = 1.
do i = 1,100
  array(i) = i
enddo
array_recieved = 0.

call mero_send_array_long(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_long(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array)])

do i = 1, size(array)
   if ( array(i) .ne. array_recieved(i) ) then
      write(*,*) 'Arrays not he same', array(i), array_recieved(i)
   endif
enddo

write(*,*) '--- long test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_long

subroutine test_2d_long()
use mero_fortran
use iso_c_binding

implicit none

integer(kind=C_LONG), allocatable :: array(:,:) ! the array to fill
type(C_PTR) :: p
integer(C_LONG), pointer :: array_recieved(:,:)
integer(kind=C_LONG) :: i,j                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_long) :: mero_send_array_long
procedure(recieve_array) :: mero_recieve_array_long

block_size = 4096

allocate(array(100,100))
allocate(array_recieved(100,100))

array = 1

do i = 1,100
  do j = 1,100
    array(i,j) = j + (i-1)*100
  enddo
enddo

array_recieved = 0

call mero_send_array_long(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_long(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array,1),size(array,2)])

do i = 1, 100
  do j = 1, 100
    if ( array(i,j) .ne. array_recieved(i,j) ) then
      write(*,*) 'Arrays not he same', array(i,j), array_recieved(i,j)
    endif
  enddo
enddo

write(*,*) '--- long 2d test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_2d_long

subroutine test_float()
use mero_fortran
use iso_c_binding

implicit none

real(kind=C_FLOAT), allocatable :: array(:) ! the array to fill
type(C_PTR) :: p
real(C_FLOAT), pointer :: array_recieved(:)
integer :: i                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_real) :: mero_send_array_float
procedure(recieve_array) :: mero_recieve_array_float

block_size = 4096

allocate(array(100))
allocate(array_recieved(100))
array = 1.
do i = 1,100
  array(i) = float(i)
enddo
array_recieved = 0.

call mero_send_array_float(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_float(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array)])

do i = 1, size(array)
   if ( array(i) .ne. array_recieved(i) ) then
      write(*,*) 'Arrays not he same', array(i), array_recieved(i)
   endif
enddo

write(*,*) '--- real test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_float

subroutine test_2d_float()
use mero_fortran
use iso_c_binding

implicit none

real(kind=C_FLOAT), allocatable :: array(:,:) ! the array to fill
type(C_PTR) :: p
real(kind=C_FLOAT), pointer :: array_recieved(:,:)
integer(kind=C_INT) :: i,j                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_real) :: mero_send_array_float
procedure(recieve_array) :: mero_recieve_array_float

block_size = 4096

allocate(array(100,100))
allocate(array_recieved(100,100))

array = 1

do i = 1,100
  do j = 1,100
    array(i,j) = float(j) + float((i-1)*100)
  enddo
enddo

array_recieved = 0

call mero_send_array_float(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_float(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array,1),size(array,2)])

do i = 1, 100
  do j = 1, 100
    if ( array(i,j) .ne. array_recieved(i,j) ) then
      write(*,*) 'Arrays not he same', array(i,j), array_recieved(i,j)
    endif
  enddo
enddo

write(*,*) '--- float 2d test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_2d_float

subroutine test_double()
use mero_fortran
use iso_c_binding

implicit none

real(kind=C_DOUBLE), allocatable :: array(:) ! the array to fill
type(C_PTR) :: p
real(C_DOUBLE), pointer :: array_recieved(:)
integer :: i                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_double) :: mero_send_array_double
procedure(recieve_array) :: mero_recieve_array_double

block_size = 4096

allocate(array(100))
allocate(array_recieved(100))
array = 1.
do i = 1,100
  array(i) = dble(i)
enddo
array_recieved = 0.

call mero_send_array_double(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_double(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array)])

do i = 1, size(array)
   if ( array(i) .ne. array_recieved(i) ) then
      write(*,*) 'Arrays not he same', array(i), array_recieved(i)
   endif
enddo

write(*,*) '--- double test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_double

subroutine test_2d_double()
use mero_fortran
use iso_c_binding

implicit none

real(kind=C_DOUBLE), allocatable :: array(:,:) ! the array to fill
type(C_PTR) :: p
real(C_DOUBLE), pointer :: array_recieved(:,:)
integer(kind=C_INT) :: i,j                     ! generic loop varable
integer(C_INT) :: array_size            ! the size of the array
integer(C_INT) :: block_size            ! size of mero block to use
integer(C_INT) :: block_count                 ! the block count
integer(C_INT64_T) :: idhi                  ! id hi
integer(C_INT64_T) :: idlo                  ! id lo

procedure(send_array_double) :: mero_send_array_double
procedure(recieve_array) :: mero_recieve_array_double

block_size = 4096

allocate(array(100,100))
allocate(array_recieved(100,100))

array = 1

do i = 1,100
  do j = 1,100
    array(i,j) = dble(j) + dble((i-1)*100.)
  enddo
enddo

array_recieved = 0

call mero_send_array_double(array,size(array),block_size,block_count,idhi,idlo)
call mero_recieve_array_double(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array,1),size(array,2)])

do i = 1, 100
  do j = 1, 100
    if ( array(i,j) .ne. array_recieved(i,j) ) then
      write(*,*) 'Arrays not he same', array(i,j), array_recieved(i,j)
    endif
  enddo
enddo

write(*,*) '--- double 2d test passed ---' 

deallocate(array)
deallocate(array_recieved)

end subroutine test_2d_double
