module mero_fortran
  abstract interface 
     ! send array
     subroutine send_array_int(array, array_size, block_size, & 
          block_count, idhi, idlo) bind(C)
       use iso_c_binding
       implicit none
       !type(C_PTR) :: array(*)
       integer(kind=C_INT) :: array(*)
       integer(kind=C_INT),value :: array_size
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT)  :: block_count
       integer(kind=C_INT64_T) :: idhi
       integer(kind=C_INT64_T) :: idlo
     end subroutine send_array_int

     subroutine recieve_array_int(array_recieved, array_length, block_size, &
          block_count, idhi, idlo ) bind(C)
       use iso_c_binding
       implicit none
       type(C_PTR) :: array_recieved 
       integer(kind=C_INT),value :: array_length
       integer(kind=C_INT),value :: block_size
       integer(kind=C_INT),value :: block_count
       integer(kind=C_INT64_T),value :: idhi
       integer(kind=C_INT64_T),value :: idlo
     end subroutine recieve_array_int
     
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

call test_int()
call test_float()

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
procedure(recieve_array_int) :: mero_recieve_array_int

block_size = 4096

allocate(array(100))
allocate(array_recieved(100))
array = 1.
do i = 1,100
  array(i) = i
enddo
array_recieved = 0.
write(*,*) '!FORTRAN!', size(array),block_size,block_count,idhi,idlo
call mero_send_array_int(array,size(array),block_size,block_count,idhi,idlo)
write(*,*) '!FORTRAN!',size(array),block_size,block_count,idhi,idlo
call mero_recieve_array_int(p, size(array), block_size, block_count, idhi, idlo)
call C_F_POINTER(p,array_recieved, [size(array)])
do i = 1, size(array)
   if ( array(i) .ne. array_recieved(i) ) then
      write(*,*) 'Arrays not he same', array(i), array_recieved(i)
   endif
   write(*,*) array(i),array_recieved(i)
enddo

deallocate(array)
deallocate(array_recieved)

end subroutine test_int

subroutine test_float()
implicit none
end subroutine test_float
