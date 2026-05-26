--- Helpers for multi-channel Protoplug scripts.
--
-- The `multiIO` global is available to every Protoplug script after including
-- the main Protoplug header:
-- 	require "include/protoplug"
-- @module multiIO

local script = require "include/core/script"
local multiIO = {}

local Channel = { }
function Channel:new (o)
	setmetatable(o, self)
	self.__index = self
	return o
end

multiIO.channels = {}

local function getChannelCount()
	return math.max(plugin.numInputChannels or 0, plugin.numOutputChannels or 0)
end

function multiIO.getNumInputs()
	return plugin.numInputChannels
end

function multiIO.getNumOutputs()
	return plugin.numOutputChannels
end

function multiIO.getNumChannels()
	return plugin.numChannels
end

function multiIO.ensureChannels()
	local nChannels = getChannelCount()
	for i = 1, nChannels do
		if multiIO.channels[i] == nil then
			multiIO.channels[i] = Channel:new{ index = i }
			if Channel.init ~= nil then
				multiIO.channels[i]:init()
			end
		end
	end
	return nChannels
end

function multiIO.init()
	if plugin.addHandler then
		plugin.addHandler("prepareToPlay", multiIO.ensureChannels)
	end

	function plugin.processBlock (samples, smax, midiBuf)
		if multiIO.processMIDI then
			multiIO.processMIDI(midiBuf)
		end

		if Channel.processBlock == nil then return 0 end

		local nChannels = multiIO.ensureChannels()
		for i = 1, nChannels do
			multiIO.channels[i]:processBlock(samples[i - 1], smax)
		end
	end
end

script.addHandler("init", function ()
	if Channel.processBlock ~= nil then
		multiIO.init()
	end
end)

multiIO.Channel = Channel

return multiIO
