//
// windows/object_handle.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2011 Boris Schaeling (boris@highscore.de)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_WINDOWS_OBJECT_HANDLE_HPP
#define BOOST_ASIO_WINDOWS_OBJECT_HANDLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_WINDOWS_OBJECT_HANDLE) \
  || defined(GENERATING_DOCUMENTATION)

#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/io_object_impl.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/detail/win_object_handle_service.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>

#if defined(BOOST_ASIO_HAS_MOVE)
# include <utility>
#endif // defined(BOOST_ASIO_HAS_MOVE)

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace windows {

/// Provides object-oriented handle functionality.
/**
 * The windows::object_handle class provides asynchronous and blocking
 * object-oriented handle functionality.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
class object_handle
{
public:
  /// The type of the executor associated with the object.
  typedef io_context::executor_type executor_type;

  /// The native representation of a handle.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_handle_type;
#else
  typedef boost::asio::detail::win_object_handle_service::native_handle_type
    native_handle_type;
#endif

  /// An object_handle is always the lowest layer.
  typedef object_handle lowest_layer_type;

  /// Construct an object_handle without opening it.
  /**
   * This constructor creates an object handle without opening it.
   *
   * @param io_context The io_context object that the object handle will use to
   * dispatch handlers for any asynchronous operations performed on the handle.
   */
  explicit object_handle(boost::asio::io_context& io_context)
    : impl_(io_context)
  {
  }

  /// Construct an object_handle on an existing native handle.
  /**
   * This constructor creates an object handle object to hold an existing native
   * handle.
   *
   * @param io_context The io_context object that the object handle will use to
   * dispatch handlers for any asynchronous operations performed on the handle.
   *
   * @param native_handle The new underlying handle implementation.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  object_handle(boost::asio::io_context& io_context,
      const native_handle_type& native_handle)
    : impl_(io_context)
  {
    boost::system::error_code ec;
    impl_.get_service().assign(impl_.get_implementation(), native_handle, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

#if defined(BOOST_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct an object_handle from another.
  /**
   * This constructor moves an object handle from one object to another.
   *
   * @param other The other object_handle object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c object_handle(io_context&) constructor.
   */
  object_handle(object_handle&& other)
    : impl_(std::move(other.impl_))
  {
  }

  /// Move-assign an object_handle from another.
  /**
   * This assignment operator moves an object handle from one object to another.
   *
   * @param other The other object_handle object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c object_handle(io_context&) constructor.
   */
  object_handle& operator=(object_handle&& other)
  {
    impl_ = std::move(other.impl_);
    return *this;
  }
#endif // defined(BOOST_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

  /// Get the executor associated with the object.
  executor_type get_executor() BOOST_ASIO_NOEXCEPT
  {
    return impl_.get_executor();
  }

  /// Get a reference to the lowest layer.
  /**
   * This function returns a reference to the lowest layer in a stack of
   * layers. Since an object_handle cannot contain any further layers, it simply
   * returns a reference to itself.
   *
   * @return A reference to the lowest layer in the stack of layers. Ownership
   * is not transferred to the caller.
   */
  lowest_layer_type& lowest_layer()
  {
    return *this;
  }

  /// Get a const reference to the lowest layer.
  /**
   * This function returns a const reference to the lowest layer in a stack of
   * layers. Since an object_handle cannot contain any further layers, it simply
   * returns a reference to itself.
   *
   * @return A const reference to the lowest layer in the stack of layers.
   * Ownership is not transferred to the caller.
   */
  const lowest_layer_type& lowest_layer() const
  {
    return *this;
  }

  /// Assign an existing native handle to the handle.
  /*
   * This function opens the handle to hold an existing native handle.
   *
   * @param handle A native handle.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void assign(const native_handle_type& handle)
  {
    boost::system::error_code ec;
    impl_.get_service().assign(impl_.get_implementation(), handle, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

  /// Assign an existing native handle to the handle.
  /*
   * This function opens the handle to hold an existing native handle.
   *
   * @param handle A native handle.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID assign(const native_handle_type& handle,
      boost::system::error_code& ec)
  {
    impl_.get_service().assign(impl_.get_implementation(), handle, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the handle is open.
  bool is_open() const
  {
    return impl_.get_service().is_open(impl_.get_implementation());
  }

  /// Close the handle.
  /**
   * This function is used to close the handle. Any asynchronous read or write
   * operations will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void close()
  {
    boost::system::error_code ec;
    impl_.get_service().close(impl_.get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "close");
  }

  /// Close the handle.
  /**
   * This function is used to close the handle. Any asynchronous read or write
   * operations will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID close(boost::system::error_code& ec)
  {
    impl_.get_service().close(impl_.get_implementation(), ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get the native handle representation.
  /**
   * This function may be used to obtain the underlying representation of the
   * handle. This is intended to allow access to native handle functionality
   * that is not otherwise provided.
   */
  native_handle_type native_handle()
  {
    return impl_.get_service().native_handle(impl_.get_implementation());
  }

  /// Cancel all asynchronous operations associated with the handle.
  /**
   * This function causes all outstanding asynchronous read or write operations
   * to finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void cancel()
  {
    boost::system::error_code ec;
    impl_.get_service().cancel(impl_.get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "cancel");
  }

  /// Cancel all asynchronous operations associated with the handle.
  /**
   * This function causes all outstanding asynchronous read or write operations
   * to finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID cancel(boost::system::error_code& ec)
  {
    impl_.get_service().cancel(impl_.get_implementation(), ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Perform a blocking wait on the object handle.
  /**
   * This function is used to wait for the object handle to be set to the
   * signalled state. This function blocks and does not return until the object
   * handle has been set to the signalled state.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void wait()
  {
    boost::system::error_code ec;
    impl_.get_service().wait(impl_.get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "wait");
  }

  /// Perform a blocking wait on the object handle.
  /**
   * This function is used to wait for the object handle to be set to the
   * signalled state. This function blocks and does not return until the object
   * handle has been set to the signalled state.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  void wait(boost::system::error_code& ec)
  {
    impl_.get_service().wait(impl_.get_implementation(), ec);
  }

  /// Start an asynchronous wait on the object handle.
  /**
   * This function is be used to initiate an asynchronous wait against the
   * object handle. It always returns immediately.
   *
   * @param handler The handler to be called when the object handle is set to
   * the signalled state. Copies will be made of the handler as required. The
   * function signature of the handler must be:
   * @code void handler(
   *   const boost::system::error_code& error // Result of operation.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * boost::asio::io_context::post().
   */
  template <typename WaitHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(WaitHandler,
      void (boost::system::error_code))
  async_wait(BOOST_ASIO_MOVE_ARG(WaitHandler) handler)
  {
    boost::asio::async_completion<WaitHandler,
      void (boost::system::error_code)> init(handler);

    impl_.get_service().async_wait(impl_.get_implementation(),
        init.completion_handler);

    return init.result.get();
  }

private:
  // Disallow copying and assignment.
  object_handle(const object_handle&) BOOST_ASIO_DELETED;
  object_handle& operator=(const object_handle&) BOOST_ASIO_DELETED;

  boost::asio::detail::io_object_impl<
    boost::asio::detail::win_object_handle_service> impl_;
};

} // namespace windows
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // defined(BOOST_ASIO_HAS_WINDOWS_OBJECT_HANDLE)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // BOOST_ASIO_WINDOWS_OBJECT_HANDLE_HPP
