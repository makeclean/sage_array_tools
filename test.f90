module mero_fortran
  abstract interface 
     ! send array
     subroutine send_array_int(array, array_size, block_size, & 
          count, idhi, idlo) bind(C)
       use iso_c_binding
       integer(kind=C_INT), allocatable, target :: array(:)
       integer(kind=C_INT) :: array_size
       integer(kind=C_INT) :: block_size
       integer(kind=C_INT) :: count
       integer(kind=C_INT) :: idhi
       integer(kind=C_INT) :: idlo
     end subroutine send_array_int

     subroutine recieve_array_int(idhi, idlo, array_length, block_size, &
          count, array_recieved) bind(C)
       use iso_c_binding
       integer(kind=C_INT), allocatable, target :: array_recieved(:)
       integer(kind=C_INT) :: array_length
       integer(kind=C_INT) :: block_size
       integer(kind=C_INT) :: count
       integer(kind=C_INT) :: idhi
       integer(kind=C_INT) :: idlo
     end subroutine recieve_array_int
  end interface
end module

program test_fortran_c
implicit none

call test_int()
call test_float()

end program test_fortran_c

subroutine test_int()
use mero_fortran

implicit none

integer, allocatable :: array(:) ! the array to fill
integer, allocatable :: array_recieved(:) ! the array to fill
integer :: i                     ! generic loop varable
integer :: array_size            ! the size of the array
integer :: block_size            ! size of mero block to use
integer :: count                 ! the block count
integer :: idhi                  ! id hi
integer :: idlo                  ! id lo

procedure(send_array_int) :: c0appz_send_array_int
procedure(recieve_array_int) :: c0appz_recieve_array_int

block_size = 4096

allocate(array(100))
allocate(array_recieved(100))
array = 1.
array_recieved = 0.

call c0appz_send_array_int(array,size(array),block_size,count,idhi,idlo)

call c0appz_recieve_array_int(idhi, idlo, size(array), block_size, count, array_recieved)

do i = 1, size(array)
   if ( array(i) .ne. array_recieved(i) ) then
      write(*,*) 'Arrays not he same'
   endif
enddo

deallocate(array)
deallocate(array_recieved)

end subroutine test_int

subroutine test_float()
implicit none
end subroutine test_float
