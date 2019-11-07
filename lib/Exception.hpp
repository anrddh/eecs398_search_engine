// Created by Jaeyoon Kim 11/6/2019
#pragma once

// Simple exception class for reporting String errors
struct Exception {
	Exception(const char* msg_) : msg(msg_)
		{}
	const char* msg;
};
