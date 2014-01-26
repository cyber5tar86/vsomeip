//
// tcp_client_impl.cpp
//
// Author: Lutz Bichler <Lutz.Bichler@bmwgroup.com>
//
// This file is part of the BMW Some/IP implementation.
//
// Copyright �� 2013, 2014 Bayerische Motoren Werke AG (BMW).
// All rights reserved.
//

#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

#include <vsomeip/endpoint.hpp>
#include <vsomeip/config.hpp>
#include <vsomeip/impl/tcp_client_impl.hpp>

namespace ip = boost::asio::ip;

namespace vsomeip {

tcp_client_impl::tcp_client_impl(const endpoint *_endpoint)
		: client_base_impl(VSOMEIP_MAX_TCP_MESSAGE_SIZE),
		  socket_(is_),
		  local_endpoint_(ip::address::from_string(_endpoint->get_address()),
				  		  _endpoint->get_port()) {
}

void tcp_client_impl::start() {
	socket_.open(local_endpoint_.protocol().v4());
	socket_.async_connect(local_endpoint_,
			boost::bind(&client_base_impl::connected, this,
					boost::asio::placeholders::error));

	// Nagle algorithm off
	ip::tcp::no_delay option;
	socket_.set_option(option);
}

void tcp_client_impl::stop() {
	if (socket_.is_open())
		socket_.close();
}

void tcp_client_impl::restart() {

}

void tcp_client_impl::send_queued() {
	boost::asio::async_write(
			socket_,
			boost::asio::buffer(&packet_queue_.front()[0], packet_queue_.front().size()),
			boost::bind(&client_base_impl::sent, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
//	socket_.async_read_some(
//			boost::asio::buffer(received_),
//			boost::bind(&client_base_impl::received, this,
//					boost::asio::placeholders::error,
//					boost::asio::placeholders::bytes_transferred));
}

std::string tcp_client_impl::get_remote_address() const {
	return local_endpoint_.address().to_string();
}

uint16_t tcp_client_impl::get_remote_port() const {
	return local_endpoint_.port();
}

ip_protocol tcp_client_impl::get_protocol() const {
	return ip_protocol::TCP;
}

ip_version tcp_client_impl::get_version() const {
	return (local_endpoint_.protocol().v4() == ip::tcp::v4() ? ip_version::V4 : ip_version::V6);
}

const uint8_t * tcp_client_impl::get_received() const {
	return received_.data();
}

} // namespace vsomeip





